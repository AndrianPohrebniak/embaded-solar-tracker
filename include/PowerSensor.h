#pragma once
#include <Arduino.h>
#include <Adafruit_INA219.h>

// Наша структура для зручної передачі даних про струм
struct PowerData {
    float voltage;
    float current;
    float power;
};

class PowerSensor {
private:
    Adafruit_INA219 ina219;
    bool isReady;

public:
    PowerSensor();
    void init();
    PowerData readAll();
    void printStats();
};