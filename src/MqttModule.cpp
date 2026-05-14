#include "MqttModule.h"
#include "Config.h"
#include <WiFi.h>

namespace {

constexpr unsigned long INITIAL_BACKOFF_MS = 2000;
constexpr unsigned long MAX_BACKOFF_MS = 30000;
constexpr unsigned long POST_DISCONNECT_GRACE_MS = 1500;
// Поріг невдалих publish() підряд, при якому loop() форсує реконект.
// Виявляє «напівмертвий» TCP-сокет швидше, ніж keepalive (~25 с замість 75 с).
constexpr unsigned int MAX_PUBLISH_FAILURES_BEFORE_RECONNECT = 3;

void spinMqttLoop(PubSubClient& client, int times) {
    for (int i = 0; i < times; ++i) {
        client.loop();
        yield();
    }
}

}  // namespace

void MqttModule::init() {
    Serial.println("\n[MQTT] Налаштування клієнта (Mosquitto)…");
    // Буфер — найперше: PubSubClient інакше алокує дефолтні 128 байт.
    mqttClient.setBufferSize(1024);
    standardClient.setTimeout(8000);
    // НЕ викликаємо тут standardClient.setNoDelay(): сокет ще не створений,
    // setsockopt() поверне EBADF (errno 9). Робимо це після успішного connect.
    mqttClient.setClient(standardClient);
    mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
    // Коротший keepAlive → PubSubClient швидше тригерить PINGREQ і виявляє
    // обрив через socketTimeout, замість 60+ секунд тиші.
    mqttClient.setKeepAlive(15);
    mqttClient.setSocketTimeout(10);
    nextConnectAllowedMs = 0;
    reconnectBackoffMs = INITIAL_BACKOFF_MS;
    hadSuccessfulConnect = false;
    consecutivePublishFailures = 0;
}

bool MqttModule::isConnected() {
    return mqttClient.connected();
}

void MqttModule::loop() {
    if (mqttClient.connected()) {
        // Якщо publish стабільно повертає false — TCP «напівмертвий».
        // Форсуємо реконект, не чекаючи keepalive-таймауту.
        if (consecutivePublishFailures >= MAX_PUBLISH_FAILURES_BEFORE_RECONNECT) {
            Serial.printf("[MQTT] %u підряд publish=false → керований реконект\n",
                consecutivePublishFailures);
            mqttClient.disconnect();
            consecutivePublishFailures = 0;
            reconnectBackoffMs = INITIAL_BACKOFF_MS;
            nextConnectAllowedMs = millis() + POST_DISCONNECT_GRACE_MS;
            return;
        }
        mqttClient.loop();
        return;
    }

    if (hadSuccessfulConnect && nextConnectAllowedMs == 0) {
        nextConnectAllowedMs = millis() + POST_DISCONNECT_GRACE_MS;
    }

    if (nextConnectAllowedMs != 0 && millis() < nextConnectAllowedMs) {
        return;
    }

    Serial.println("\n[MQTT] Стукаємо до Mosquitto…");
    Serial.printf("[MQTT] з ESP: ip=%s ціль: %s:%d (backoff=%lu ms)\n",
        WiFi.localIP().toString().c_str(),
        MQTT_BROKER,
        MQTT_PORT,
        reconnectBackoffMs);

    String mac = WiFi.macAddress();
    mac.replace(":", "");
    String clientId = "esp32-" + mac;

    if (mqttClient.connect(clientId.c_str())) {
        // Тепер сокет створено — тільки тут setNoDelay реально вмикає TCP_NODELAY.
        standardClient.setNoDelay(true);
        Serial.println("[MQTT] ✅ ПІДКЛЮЧЕНО УСПІШНО ДО Mosquitto!");
        hadSuccessfulConnect = true;
        reconnectBackoffMs = INITIAL_BACKOFF_MS;
        nextConnectAllowedMs = 0;
        consecutivePublishFailures = 0;
        spinMqttLoop(mqttClient, 5);
    } else {
        const int st = mqttClient.state();
        Serial.printf("[MQTT] ❌ rc=%d, наступна спроба через %lu ms\n", st, reconnectBackoffMs);
        nextConnectAllowedMs = millis() + reconnectBackoffMs;
        unsigned long nextBackoff = reconnectBackoffMs * 2;
        if (nextBackoff > MAX_BACKOFF_MS) {
            nextBackoff = MAX_BACKOFF_MS;
        }
        if (nextBackoff < INITIAL_BACKOFF_MS) {
            nextBackoff = INITIAL_BACKOFF_MS;
        }
        reconnectBackoffMs = nextBackoff;
    }
}

bool MqttModule::publishTelemetry(const LightData& light, float panDeg, float tiltDeg, const PowerData& pwr) {
    if (!mqttClient.connected()) {
        return false;
    }

    char payload[320];
    const int n = snprintf(
        payload,
        sizeof(payload),
        "{\"pan\":%.2f,\"tilt\":%.2f,\"tl\":%d,\"tr\":%d,\"dl\":%d,\"dr\":%d,"
        "\"v\":%.2f,\"i\":%.2f,\"p\":%.2f}",
        panDeg,
        tiltDeg,
        light.tl,
        light.tr,
        light.dl,
        light.dr,
        pwr.voltage,
        pwr.current,
        pwr.power);
    if (n <= 0 || static_cast<size_t>(n) >= sizeof(payload)) {
        return false;
    }

    if (mqttClient.publish(MQTT_TOPIC, payload)) {
        consecutivePublishFailures = 0;
        return true;
    }

    // Один м'який ретрай без spin/disconnect.
    yield();
    if (mqttClient.publish(MQTT_TOPIC, payload)) {
        consecutivePublishFailures = 0;
        return true;
    }

    consecutivePublishFailures++;
    Serial.printf("[MQTT] publish=false (підряд=%u, state=%d)\n",
        consecutivePublishFailures, mqttClient.state());
    // Реконект тут не робимо: коли лічильник перетне поріг,
    // MqttModule::loop() форсує disconnect і чисту реконект-сесію.
    return false;
}
