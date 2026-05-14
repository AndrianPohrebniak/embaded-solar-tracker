#include "Tracker.h"
#include "Config.h"
#include <cmath>

Tracker::Tracker() {
    panAngle = static_cast<float>(PARK_ANGLE_PAN);
    tiltAngle = static_cast<float>(PARK_ANGLE_TILT);
}

void Tracker::init() {
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    panServo.setPeriodHertz(50);
    tiltServo.setPeriodHertz(50);

    panServo.attach(PIN_PAN, 500, 2400);
    tiltServo.attach(PIN_TILT, 500, 2400);

    panServo.write(static_cast<int>(panAngle + 0.5f));
    tiltServo.write(static_cast<int>(tiltAngle + 0.5f));
}

void Tracker::performTracking(LightData data) {
    const int left = (data.tl + data.dl) / 2;
    const int right = (data.tr + data.dr) / 2;
    const int top = (data.tl + data.tr) / 2;
    const int bot = (data.dl + data.dr) / 2;

    const int diffHoriz = left - right;
    const int diffVert = top - bot;

    const float tol = static_cast<float>(TOLERANCE);

    const float magH = std::fabs(static_cast<float>(diffHoriz));
    const float scaleH = (magH > tol) ? 1.f : (magH / tol);
    float stepPan = static_cast<float>(diffHoriz) * KP_PAN * scaleH;
    stepPan = constrain(stepPan, -MAX_STEP, MAX_STEP);
    panAngle += stepPan;

    const float magV = std::fabs(static_cast<float>(diffVert));
    const float scaleV = (magV > tol) ? 1.f : (magV / tol);
    float stepTilt = static_cast<float>(diffVert) * KP_TILT * scaleV;
    stepTilt = constrain(stepTilt, -MAX_STEP, MAX_STEP);
    tiltAngle += stepTilt;

    panAngle = constrain(panAngle, static_cast<float>(PAN_MIN), static_cast<float>(PAN_MAX));
    tiltAngle = constrain(tiltAngle, static_cast<float>(TILT_MIN), static_cast<float>(TILT_MAX));

    panServo.write(static_cast<int>(panAngle + 0.5f));
    tiltServo.write(static_cast<int>(tiltAngle + 0.5f));
}
