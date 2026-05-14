#pragma once
#include <Adafruit_SSD1306.h>
#include "PowerSensor.h" 

class DisplayModule {
private:
    Adafruit_SSD1306 display;
    bool isReady;

public:
    DisplayModule();
    void init();
    void update(PowerData data); 
};