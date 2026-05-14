#include <Arduino.h>
#include "Config.h"
#include "LightArray.h"
#include "Tracker.h"
#include "PowerSensor.h"
#include "DisplayModule.h"
#include "WiFiModule.h"
#include "MqttModule.h"

LightArray sensors;
Tracker solarTracker;
PowerSensor powerMeter;
DisplayModule oled;
WiFiModule wifi;
MqttModule mqtt;

unsigned long lastPublishMs = 0;
unsigned long lastDisplayMs = 0;

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n=== SOLAR TRACKER (HiveMQ TLS) ===");

    sensors.init();
    solarTracker.init();
    powerMeter.init();
    oled.init();

    wifi.init();
    mqtt.init();
    mqtt.connect();

    Serial.println("\n--- SOLAR TRACKER READY ---");
}

void loop() {
    if (!wifi.isConnected()) {
        Serial.println("[WiFi] втрата зв'язку → reconnect");
        wifi.init();
    }

    if (!mqtt.isConnected()) {
        mqtt.connect();
        delay(2000);
        return;
    }

    mqtt.loop();

    LightData currentLight = sensors.readAll();
    solarTracker.performTracking(currentLight);
    delay(STEP_DELAY);

    mqtt.loop();

    if (millis() - lastDisplayMs >= 500) {
        lastDisplayMs = millis();
        PowerData pwr = powerMeter.readAll();
        oled.update(pwr);
    }

    if (millis() - lastPublishMs >= MQTT_PUBLISH_INTERVAL_MS) {
        lastPublishMs = millis();
        LightData ld = sensors.readAll();
        PowerData p = powerMeter.readAll();
        mqtt.publishTelemetry(ld, solarTracker.getPanAngle(), solarTracker.getTiltAngle(), p);
    }
}
