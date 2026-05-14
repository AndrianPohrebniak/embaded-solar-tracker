#pragma once
#include <Arduino.h>
#include <ESP32Servo.h>
#include "LightArray.h"

class Tracker {
private:
    Servo panServo;
    Servo tiltServo;
    float panAngle;
    float tiltAngle;

public:
    Tracker();
    void init();
    void performTracking(LightData data);
    float getPanAngle() const { return panAngle; }
    float getTiltAngle() const { return tiltAngle; }
};
