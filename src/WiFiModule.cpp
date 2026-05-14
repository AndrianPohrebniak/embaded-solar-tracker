#include "WiFiModule.h"
#include "Config.h"
#include <esp_wifi.h>

void WiFiModule::init() {
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

bool WiFiModule::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}
