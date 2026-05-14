#pragma once
#include <Arduino.h>
#include <ESP32Servo.h>
#include <Preferences.h>
#include "LightArray.h"

class Tracker {
private:
    Servo panServo;
    Servo tiltServo;
    float panAngle;
    float tiltAngle;

    // Збереження останніх кутів у NVS (флеш ESP32), щоб після рестарту
    // сервоприводи стартували з останньої відомої позиції, а не з PARK.
    Preferences prefs;
    float lastSavedPan;
    float lastSavedTilt;
    unsigned long lastSaveMs;

    void saveIfNeeded();

public:
    Tracker();
    void init();
    void performTracking(LightData data);
    float getPanAngle() const { return panAngle; }
    float getTiltAngle() const { return tiltAngle; }
};
