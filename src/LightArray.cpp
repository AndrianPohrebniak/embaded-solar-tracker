#include "LightArray.h"
#include "Config.h"

void LightArray::init() {
}

LightData LightArray::readAll() {
    LightData data;
    
    data.tl = analogRead(PIN_TL);
    data.tr = analogRead(PIN_TR);
    data.dl = analogRead(PIN_DL);
    data.dr = analogRead(PIN_DR);
    
    data.average = (data.tl + data.tr + data.dl + data.dr) / 4;
    data.isNight = (data.average > NIGHT_THRESHOLD);
    
    return data;
}