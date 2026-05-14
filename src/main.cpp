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

unsigned long lastUpdateTime = 0;
unsigned long lastMqttPublishMs = 0;

void setup()
{
    Serial.begin(115200);
    delay(1000);

    // Ініціалізація заліза
    sensors.init();
    solarTracker.init();
    powerMeter.init();
    oled.init();

    // Ініціалізація мережі
    wifi.init();
    delay(800);
    mqtt.init();

    Serial.println("\n--- SOLAR TRACKER READY ---");
}

void loop()
{
    // Спочатку MQTT — щоб keepalive/PING оброблялись до delay() і важкої роботи.
    if (wifi.isConnected()) {
        mqtt.loop();
    }

    // 1. Керування трекером (працює постійно з інтервалом STEP_DELAY)
    LightData currentLight = sensors.readAll();
    solarTracker.performTracking(currentLight);
    delay(STEP_DELAY);

    if (wifi.isConnected()) {
        mqtt.loop();
    }

    // 3. Оновлення OLED-екрану (кожні 500 мс)
    if (millis() - lastUpdateTime >= 500)
    {
        lastUpdateTime = millis();
        PowerData pwr = powerMeter.readAll(); // Зчитуємо енергію тільки для екрану
        oled.update(pwr);
    }

    // 4. Відправка телеметрії в хмару HiveMQ (кожні MQTT_PUBLISH_INTERVAL_MS)
    if (wifi.isConnected() && mqtt.isConnected() && (millis() - lastMqttPublishMs >= MQTT_PUBLISH_INTERVAL_MS))
    {
        LightData ld = sensors.readAll();
        PowerData p = powerMeter.readAll();
        if (mqtt.publishTelemetry(ld, solarTracker.getPanAngle(), solarTracker.getTiltAngle(), p)) {
            lastMqttPublishMs = millis();
        } else if (mqtt.isConnected()) {
            lastMqttPublishMs = millis();
        }
        mqtt.loop();
    }
}