#include "WiFiModule.h"
#include "Config.h"

void WiFiModule::init() {
    Serial.print("\n[WiFi] Підключення до: ");
    Serial.println(WIFI_SSID);

    
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false); // щоб WiFi не спала доки не підключиться(енергозберігаючий режим)
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\n[WiFi] ✅ ПІДКЛЮЧЕНО УСПІШНО!");
    Serial.print("[WiFi] IP-адреса: ");
    Serial.println(WiFi.localIP());

    // Після асоціації гарантовано вимикаємо modem-sleep: без цього ESP32
    // періодично «засинає» і MQTT keepalive / publish зриваються.
    WiFi.setSleep(false);
}

bool WiFiModule::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}