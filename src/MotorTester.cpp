#include "MotorTester.h"

MotorTester::MotorTester(int pan_pin, int tilt_pin) {
    panPin = pan_pin;
    tiltPin = tilt_pin;
    currentPan = 90;
    currentTilt = 90;
}

void MotorTester::init() {
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    panServo.setPeriodHertz(50);
    tiltServo.setPeriodHertz(50);

    panServo.attach(panPin, 500, 2400);
    tiltServo.attach(tiltPin, 500, 2400);

    panServo.write(currentPan);
    tiltServo.write(currentTilt);

    Serial.println("\n==================================");
    Serial.println("🛠 CALIBRATION MODE ACTIVE 🛠");
    Serial.println("==================================");
    Serial.println("Як керувати:");
    Serial.println("  hXXX - Горизонталь (наприклад: h90, h120, h45)");
    Serial.println("  vXXX - Вертикаль   (наприклад: v90, v150, v180)");
    Serial.println("==================================\n");
}

void MotorTester::processCommands() {
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        input.trim();
        input.toLowerCase();

        if (input.length() > 1) {
            char axis = input.charAt(0);
            int angle = input.substring(1).toInt();
            angle = constrain(angle, 0, 180);

            if (axis == 'h') {
                currentPan = angle;
                panServo.write(currentPan);
                Serial.printf("➔ Горизонталь (PAN) встановлено на: %d°\n", currentPan);
            } 
            else if (axis == 'v') {
                currentTilt = angle;
                tiltServo.write(currentTilt);
                Serial.printf("➔ Вертикаль (TILT) встановлено на: %d°\n", currentTilt);
            } 
            else {
                Serial.println("❌ Помилка: Команда має починатися з 'h' або 'v'!");
            }
        }
    }
}