#include "MqttModule.h"
#include "Config.h"
#include <WiFi.h>

void MqttModule::init() {
    secureClient.setInsecure();
    mqttClient.setClient(secureClient);
    mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
    mqttClient.setKeepAlive(60);
    mqttClient.setSocketTimeout(15);
    mqttClient.setBufferSize(1024);
    publishCounter = 0;
}

void MqttModule::connect() {
    String clientId = String("esp32-tracker-") + String(random(0xffff), HEX);
    Serial.printf("[MQTT] Connect → %s:%d as %s (user=%s)\n",
        MQTT_BROKER, MQTT_PORT, clientId.c_str(), MQTT_USER);

    if (mqttClient.connect(clientId.c_str(), MQTT_USER, MQTT_PASS)) {
        Serial.println("[MQTT] ✅ CONNECTED to HiveMQ");
    } else {
        Serial.printf("[MQTT] ❌ rc=%d\n", mqttClient.state());
    }
}

bool MqttModule::isConnected() {
    return mqttClient.connected();
}

void MqttModule::loop() {
    mqttClient.loop();
}

bool MqttModule::publishTelemetry(const LightData& light, float panDeg, float tiltDeg, const PowerData& pwr) {
    if (!mqttClient.connected()) {
        return false;
    }

    publishCounter++;

    char payload[320];
    const int n = snprintf(
        payload,
        sizeof(payload),
        "{\"pan\":%.2f,\"tilt\":%.2f,\"tl\":%d,\"tr\":%d,\"dl\":%d,\"dr\":%d,"
        "\"v\":%.2f,\"i\":%.2f,\"p\":%.2f}",
        panDeg,
        tiltDeg,
        light.tl, light.tr, light.dl, light.dr,
        pwr.voltage, pwr.current, pwr.power);

    if (n <= 0 || n >= static_cast<int>(sizeof(payload))) {
        return false;
    }

    bool ok = mqttClient.publish(MQTT_TOPIC, payload);
    Serial.printf("[MQTT] publish #%lu → %s (RSSI=%d, heap=%u)\n",
        publishCounter,
        ok ? "OK" : "FAIL",
        WiFi.RSSI(),
        ESP.getFreeHeap());
    return ok;
}
