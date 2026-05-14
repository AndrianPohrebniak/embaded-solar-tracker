#include "PowerSensor.h"

PowerSensor::PowerSensor() {
    isReady = false;
}

void PowerSensor::init() {
    // Ініціалізуємо датчик (він працює по пінах I2C: SDA=21, SCL=22)
    if (!ina219.begin()) {
        Serial.println("❌ ПОМИЛКА: Датчик INA219 не знайдено! Перевір дроти.");
        // Не зависаємо намертво, просто фіксуємо, що датчика немає
        isReady = false; 
    } else {
        Serial.println("✅ INA219 (Лічильник енергії) підключено успішно!");
        isReady = true;
    }
}

PowerData PowerSensor::readAll() {
    PowerData data = {0.0, 0.0, 0.0}; // Пустий пакет за замовчуванням
    
    if (isReady) {
        data.voltage = ina219.getBusVoltage_V();
        data.current = ina219.getCurrent_mA();
        data.power = ina219.getPower_mW();
    }
    return data;
}

void PowerSensor::printStats() {
    if (!isReady) return;

    PowerData currentStats = readAll();
    Serial.printf("⚡ ЕНЕРГІЯ | Напруга: %.2f V | Струм: %.2f mA | Потужність: %.2f mW\n", 
                  currentStats.voltage, currentStats.current, currentStats.power);
}