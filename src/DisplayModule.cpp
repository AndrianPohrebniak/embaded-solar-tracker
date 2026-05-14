#include "DisplayModule.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1 
#define SCREEN_ADDRESS 0x3C 

DisplayModule::DisplayModule() : display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET) {
    isReady = false;
}

void DisplayModule::init() {
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("❌ Помилка OLED: Дисплей не знайдено"));
        isReady = false;
    } else {
        display.clearDisplay();
        display.setTextColor(SSD1306_WHITE);
        display.display();
        isReady = true;
    }
}

void DisplayModule::update(PowerData data) {
    if (!isReady) return;

    display.clearDisplay();
    
    // Вмикаємо великий шрифт для всіх рядків
    display.setTextSize(2);

    // Рядок 1: Напруга (V)
    display.setCursor(0, 4);
    display.print("V: "); 
    display.print(data.voltage, 2); 
    
    // Рядок 2: Струм (mA)
    display.setCursor(0, 26);
    display.print("I: "); 
    display.print(data.current, 1); 

    // Рядок 3: Потужність (mW)
    display.setCursor(0, 48);
    display.print("P: "); 
    display.print(data.power, 1); 

    display.display(); 
}