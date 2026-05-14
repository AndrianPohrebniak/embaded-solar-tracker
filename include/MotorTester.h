#ifndef MOTOR_TESTER_H
#define MOTOR_TESTER_H

#include <Arduino.h>
#include <ESP32Servo.h>

class MotorTester {
private:
    Servo panServo;
    Servo tiltServo;
    int panPin;
    int tiltPin;
    int currentPan;
    int currentTilt;

public:
    // Конструктор
    MotorTester(int pan_pin, int tilt_pin);
    
    // Методи
    void init();
    void processCommands();
};

#endif