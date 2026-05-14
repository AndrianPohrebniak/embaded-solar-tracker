#pragma once
#include <Arduino.h>
#include <WiFi.h>

class WiFiModule {
public:
    void init();
    bool isConnected();
};
