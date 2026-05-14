#pragma once
#include <Arduino.h>

struct LightData {
    int tl;
    int tr;
    int dl;
    int dr;
    int average;
    bool isNight;
};

class LightArray {
public:
    void init();
    LightData readAll();
};