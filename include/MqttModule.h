#pragma once
#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "LightArray.h"
#include "PowerSensor.h"

class MqttModule {
private:
    WiFiClientSecure secureClient;
    PubSubClient mqttClient;
    unsigned long publishCounter = 0;

public:
    void init();
    void connect();
    bool isConnected();
    void loop();

    bool publishTelemetry(const LightData& light, float panDeg, float tiltDeg, const PowerData& pwr);
};
