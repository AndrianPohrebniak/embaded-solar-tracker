#include "Tracker.h"
#include "Config.h"
#include <cmath>

namespace {

constexpr const char* PREFS_NAMESPACE = "tracker";
constexpr const char* PREFS_KEY_PAN = "pan";
constexpr const char* PREFS_KEY_TILT = "tilt";

// Зберігаємо у NVS лише якщо кут змінився на ≥ 2° від останнього збереження
// І минув мінімальний інтервал. Це дає <1 запис/хв при нормальному трекінгу
// і драматично продовжує життя флеш-памʼяті ESP32.
constexpr float SAVE_THRESHOLD_DEG = 2.0f;
constexpr unsigned long MIN_SAVE_INTERVAL_MS = 5000;

}  // namespace

Tracker::Tracker() {
    panAngle = static_cast<float>(PARK_ANGLE_PAN);
    tiltAngle = static_cast<float>(PARK_ANGLE_TILT);
    lastSavedPan = panAngle;
    lastSavedTilt = tiltAngle;
    lastSaveMs = 0;
}

void Tracker::init() {
    // Перед attach() серво читаємо останню позицію з NVS, щоб servo.write()
    // одразу пішов туди, а не у PARK_ANGLE — інакше панель смикне.
    prefs.begin(PREFS_NAMESPACE, false);
    const float savedPan = prefs.getFloat(PREFS_KEY_PAN, NAN);
    const float savedTilt = prefs.getFloat(PREFS_KEY_TILT, NAN);

    if (!std::isnan(savedPan) && savedPan >= PAN_MIN && savedPan <= PAN_MAX) {
        panAngle = savedPan;
        Serial.printf("[Tracker] Відновлено pan з NVS: %.2f°\n", panAngle);
    } else {
        Serial.printf("[Tracker] Pan не знайдено в NVS, PARK: %.2f°\n", panAngle);
    }

    if (!std::isnan(savedTilt) && savedTilt >= TILT_MIN && savedTilt <= TILT_MAX) {
        tiltAngle = savedTilt;
        Serial.printf("[Tracker] Відновлено tilt з NVS: %.2f°\n", tiltAngle);
    } else {
        Serial.printf("[Tracker] Tilt не знайдено в NVS, PARK: %.2f°\n", tiltAngle);
    }

    lastSavedPan = panAngle;
    lastSavedTilt = tiltAngle;
    lastSaveMs = millis();

    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    panServo.setPeriodHertz(50);
    tiltServo.setPeriodHertz(50);

    panServo.attach(PIN_PAN, 500, 2400);
    tiltServo.attach(PIN_TILT, 500, 2400);

    panServo.write(static_cast<int>(panAngle + 0.5f));
    tiltServo.write(static_cast<int>(tiltAngle + 0.5f));
}

void Tracker::saveIfNeeded() {
    const float dPan = std::fabs(panAngle - lastSavedPan);
    const float dTilt = std::fabs(tiltAngle - lastSavedTilt);

    if (dPan < SAVE_THRESHOLD_DEG && dTilt < SAVE_THRESHOLD_DEG) {
        return;
    }
    if (millis() - lastSaveMs < MIN_SAVE_INTERVAL_MS) {
        return;
    }

    prefs.putFloat(PREFS_KEY_PAN, panAngle);
    prefs.putFloat(PREFS_KEY_TILT, tiltAngle);
    lastSavedPan = panAngle;
    lastSavedTilt = tiltAngle;
    lastSaveMs = millis();
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

    saveIfNeeded();
}
