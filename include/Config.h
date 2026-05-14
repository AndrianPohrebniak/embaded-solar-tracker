#pragma once
#include <Arduino.h>

// --- Піни сервомоторів ---
const int PIN_PAN = 15;
const int PIN_TILT = 4;

// --- Піни фоторезисторів ---
const int PIN_TL = 35; // Top Left
const int PIN_TR = 32; // Top Right
const int PIN_DL = 34; // Down Left
const int PIN_DR = 33; // Down Right

// --- Налаштування механіки ---
const int PARK_ANGLE_PAN = 90; 
const int PARK_ANGLE_TILT = 35;
const int TILT_MIN = 10;
const int TILT_MAX = 75;
const int PAN_MIN = 10;
const int PAN_MAX = 170;

const float KP_PAN = 0.02f;
const float KP_TILT = 0.02f;
// Поріг чутливості (ADC): занадто великий — панорама не рухається при малих dH (напр. <100).
const int TOLERANCE = 120;
// Затримка між кроками трекінгу (мс). Менше — частіше оновлення сервоприводів.
const int STEP_DELAY = 40;

// Макс. зміна кута за один крок (градуси). Більше — швидший рух при великій різниці світла.
const float MAX_STEP = 0.82f;

// --- Налаштування середовища ---
const int NIGHT_THRESHOLD = 3800;

// --- Налаштування Wi-Fi ---
static constexpr const char* WIFI_SSID = "Andrian";
static constexpr const char* WIFI_PASS = "yumira20";

// --- HiveMQ Cloud (MQTT over TLS, порт 8883) ---
static constexpr const char* MQTT_BROKER = "34d4937c7a8f4488b256bc82e49d4a96.s1.eu.hivemq.cloud";
static constexpr int MQTT_PORT = 8883;
static constexpr const char* MQTT_USER = "andrian12345678And";
static constexpr const char* MQTT_PASS = "andrian12345678And";

// Локальний Mosquitto (plain MQTT, порт 1883) — наразі не використовується:
// static constexpr const char* MQTT_BROKER = "192.168.0.138";
// static constexpr int MQTT_PORT = 1883;

static constexpr const char* MQTT_TOPIC = "andrian_diploma_2026/tracker/telemetry";
static constexpr unsigned long MQTT_PUBLISH_INTERVAL_MS = 2000;