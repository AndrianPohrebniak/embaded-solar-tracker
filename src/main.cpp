#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <esp_wifi.h>
#include "Config.h"

WiFiClientSecure netClient;
PubSubClient mqtt(netClient);

unsigned long lastPublishMs = 0;
unsigned long publishCounter = 0;

void connectWiFi() {
    Serial.printf("[WiFi] Підключаюсь до %s\n", WIFI_SSID);

    WiFi.persistent(false);
    WiFi.setAutoReconnect(true);
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.printf("\n[WiFi] OK ip=%s RSSI=%d dBm ch=%d\n",
        WiFi.localIP().toString().c_str(), WiFi.RSSI(), WiFi.channel());

    esp_err_t e1 = esp_wifi_set_ps(WIFI_PS_NONE);
    esp_err_t e2 = esp_wifi_set_max_tx_power(78);
    Serial.printf("[WiFi] ps=0x%x tx_power=0x%x\n", e1, e2);
}

void connectMqtt() {
    String clientId = String("esp32-hello-") + String(random(0xffff), HEX);
    Serial.printf("[MQTT] Connect → %s:%d as %s (user=%s)\n",
        MQTT_BROKER, MQTT_PORT, clientId.c_str(), MQTT_USER);

    if (mqtt.connect(clientId.c_str(), MQTT_USER, MQTT_PASS)) {
        Serial.println("[MQTT] ✅ CONNECTED to HiveMQ");
    } else {
        Serial.printf("[MQTT] ❌ rc=%d\n", mqtt.state());
    }
}

void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("\n=== HELLO-WORLD MQTT TEST (HiveMQ TLS) ===");

    connectWiFi();

    netClient.setInsecure();

    mqtt.setServer(MQTT_BROKER, MQTT_PORT);
    mqtt.setKeepAlive(60);
    mqtt.setSocketTimeout(15);
    mqtt.setBufferSize(1024);

    connectMqtt();
}

void loop() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[WiFi] втрата зв'язку → reconnect");
        connectWiFi();
    }

    if (!mqtt.connected()) {
        connectMqtt();
        delay(2000);
        return;
    }

    mqtt.loop();

    if (millis() - lastPublishMs >= 2000) {
        lastPublishMs = millis();
        publishCounter++;

        char payload[64];
        snprintf(payload, sizeof(payload), "Hello World #%lu", publishCounter);

        bool ok = mqtt.publish(MQTT_TOPIC, payload);
        Serial.printf("[MQTT] publish #%lu → %s (RSSI=%d, heap=%u)\n",
            publishCounter,
            ok ? "OK" : "FAIL",
            WiFi.RSSI(),
            ESP.getFreeHeap());
    }
}
