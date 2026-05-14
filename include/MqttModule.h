#pragma once
#include <Arduino.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include "LightArray.h"
#include "PowerSensor.h"

class MqttModule {
private:
    WiFiClient standardClient;
    PubSubClient mqttClient;
    unsigned long nextConnectAllowedMs = 0;
    unsigned long reconnectBackoffMs = 2000;
    bool hadSuccessfulConnect = false;
    // Кількість publish() підряд, що повернули false (для м'якої політики реконекту)
    unsigned int consecutivePublishFailures = 0;

public:
    void init();
    void loop();
    bool isConnected();
    
    bool publishTelemetry(const LightData& light, float panDeg, float tiltDeg, const PowerData& pwr);
};