/*
  Smart Room Automation System using ESP32 and Blynk IoT

  Features:
  - Motion detection with PIR sensor to control lighting.
  - Ambient brightness detection using LDR sensor.
  - Temperature monitoring with DHT22 sensor to control AC relay.
  - Real-time clock (DS3231) to schedule sleep mode during off hours.
  - Manual and remote control via physical switch and Blynk app.
  - LCD display for status and time.

  Author: Narmin Othman (Freshly graduated engineer)
  Date: August 2025
*/

// ==== [Blynk IoT Configuration] ====
#define BLYNK_TEMPLATE_ID "TMPL66mF_yaYl"
#define BLYNK_TEMPLATE_NAME "WOWKI"
#define BLYNK_DEVICE_NAME "Smart lightning"
#define BLYNK_AUTH_TOKEN  "PdeKhLzN1cvFyaLiw9z5v7-Hltli-gCi"

// ==== [Library Inclusions] ====
#include <TaskScheduler.h> 
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include "DHTesp.h"
#include "RTClib.h"
#include <LiquidCrystal_I2C.h>
#include "esp_sleep.h"

// ==== [WiFi Credentials] ====
const char ssid[] = "Wokwi-GUEST";
const char pass[] = "";

// ==== [Pin Configuration] ====
constexpr int DHT22_SENSOR_PIN = 32;
constexpr int PIR_SENSOR_PIN   = 34;
constexpr int LDR_SENSOR_PIN   = 35;
constexpr int LED_PIN          = 19;
constexpr int SWITCH_PIN       = 23;
constexpr int RELAY_AC_PIN     = 18;

// ==== [Thresholds & Constants] ====
constexpr int BRIGHTNESS_THRESHOLD = 170;  // Analog threshold for LDR
constexpr float TEMP_HIGH = 28.0;
constexpr float TEMP_LOW  = 24.0;
constexpr uint64_t SLEEP_DURATION_US = 3000000ULL; // 3 seconds

// ==== [Objects & Devices] ====
LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS3231 rtc;
DHTesp dhtSensor;

// ==== [State Variables] ====
bool blynkLightOverride = false;
bool blynkACOverride = false;
bool acRelayState = false;
volatile bool motionDetected = false;

enum DisplayState {
  DISPLAY_MANUAL,
  DISPLAY_BLYNK,
  DISPLAY_MOTION,
  DISPLAY_NO_MOTION,
  DISPLAY_BRIGHT
};
DisplayState currentDisplayState;

String lastLCDMessage = "";

// ==== [TaskScheduler Setup] ====
// TaskScheduler handles non-blocking timed execution
Scheduler runner;

// ==== [Task Prototypes] ====
void taskBlynkRun();
void taskUpdateSensors();
void taskUpdateOutputs();
void taskSleepCheck();

// ==== [Task Definitions] ====
// Timing values in ms; TASK_FOREVER = run indefinitely
Task taskBlynk(10, TASK_FOREVER, &taskBlynkRun);
Task taskSensors(1000, TASK_FOREVER, &taskUpdateSensors);
Task taskOutputs(500, TASK_FOREVER, &taskUpdateOutputs);
Task taskSleep(60000, TASK_FOREVER, &taskSleepCheck);  // Once per minute

// ==== [Function Prototypes] ====
void initializeHardware();
void updateLED(bool motion, bool isBright, bool manualOverride);
void updateAC(float temp, bool motion);
void updateLCD(const DateTime& now);
void updateLCDStatus(DisplayState state, bool acOn);
float readTemperature();
void displayTime(const DateTime& now);
void IRAM_ATTR pirISR();

// ==== [Blynk Virtual Pin Handlers] ====
// Virtual pin V0 controls manual light override
BLYNK_WRITE(V0) {
  blynkLightOverride = param.asInt();
  Serial.printf("Blynk Light Override: %d\n", blynkLightOverride);
}

// Virtual pin V1 controls manual AC override
BLYNK_WRITE(V1) {
  blynkACOverride = param.asInt();
  Serial.printf("Blynk AC Override: %d\n", blynkACOverride);
  if (!blynkACOverride) {
    acRelayState = false;
    digitalWrite(RELAY_AC_PIN, HIGH); // Turn off relay
  }
}

// ==== [Setup] ====
void setup() {
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  initializeHardware();

  // Initialize and register tasks
  runner.init();
  runner.addTask(taskBlynk);
  runner.addTask(taskSensors);
  runner.addTask(taskOutputs);
  runner.addTask(taskSleep);

  taskBlynk.enable();
  taskSensors.enable();
  taskOutputs.enable();
  taskSleep.enable();
}

// ==== [Main Loop] ====
void loop() {
  runner.execute(); // Executes scheduled tasks
}

// ==== [Hardware Initialization] ====
void initializeHardware() {
  pinMode(PIR_SENSOR_PIN, INPUT);
  pinMode(DHT22_SENSOR_PIN, INPUT);
  pinMode(LDR_SENSOR_PIN, INPUT);
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  pinMode(RELAY_AC_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(RELAY_AC_PIN, HIGH); // Relay OFF at start

  attachInterrupt(digitalPinToInterrupt(PIR_SENSOR_PIN), pirISR, RISING);

  dhtSensor.setup(DHT22_SENSOR_PIN, DHTesp::DHT22);
  lcd.init();
  lcd.backlight();
  Wire.begin();

  if (!rtc.begin()) {
    lcd.print("RTC not found!");
    while (true);
  }
  if (rtc.lostPower()) {
    lcd.print("RTC Reset...");
    rtc.adjust(DateTime(2025, 8, 1, 16, 0, 0));
    delay(2000);
  }
  lcd.clear();
}

// ==== [ISR for PIR Sensor] ====
// Only sets a flag; actual logic is handled by taskUpdateSensors()
void IRAM_ATTR pirISR() {
  Serial.println("PIR Interrupt: Motion started");
}

// ==== [Task Functions] ====

// Task: Run Blynk
void taskBlynkRun() {
  Blynk.run();
}

// Task: Update motion sensor status
void taskUpdateSensors() {
  motionDetected = (digitalRead(PIR_SENSOR_PIN) == HIGH);
}

// Task: Evaluate conditions and update outputs
void taskUpdateOutputs() {
  DateTime now = rtc.now();
  bool manualOverride = (digitalRead(SWITCH_PIN) == LOW);
  bool isBright = (analogRead(LDR_SENSOR_PIN) < BRIGHTNESS_THRESHOLD);
  float temperature = readTemperature();

  updateLED(motionDetected, isBright, manualOverride);

  // AC logic
  if (blynkACOverride) {
    digitalWrite(RELAY_AC_PIN, LOW);
    Serial.println("AC ON (Manual Blynk)");
    acRelayState = true;
  } else {
    updateAC(temperature, motionDetected);
  }

  // Update display state
  if (manualOverride) currentDisplayState = DISPLAY_MANUAL;
  else if (blynkLightOverride) currentDisplayState = DISPLAY_BLYNK;
  else if (motionDetected) currentDisplayState = DISPLAY_MOTION;
  else if (isBright) currentDisplayState = DISPLAY_BRIGHT;
  else currentDisplayState = DISPLAY_NO_MOTION;

  updateLCD(now);
}

// Task: Check for off-hours and enter light sleep
void taskSleepCheck() {
  DateTime now = rtc.now();
  if (now.hour() < 6 || now.hour() > 23) {
    Serial.println("Sleeping (off hours)...");
    esp_sleep_enable_timer_wakeup(SLEEP_DURATION_US);
    esp_light_sleep_start();
  }
}

// ==== [Helper Functions] ====

// Logic for controlling LED
void updateLED(bool motion, bool isBright, bool manualOverride) {
  if (blynkLightOverride || manualOverride) {
    digitalWrite(LED_PIN, HIGH);
  } else if (!isBright && motion) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
}

// AC relay logic with hysteresis
void updateAC(float temp, bool motion) {
  if (motion && !acRelayState && temp > TEMP_HIGH) {
    acRelayState = true;
    digitalWrite(RELAY_AC_PIN, LOW);
    Serial.println("AC ON (Temp + Motion)");
  } else if (acRelayState && (temp < TEMP_LOW || !motion)) {
    acRelayState = false;
    digitalWrite(RELAY_AC_PIN, HIGH);
    Serial.println("AC OFF (Temp Low or No Motion)");
  }
}

// LCD display time and status
void updateLCD(const DateTime& now) {
  displayTime(now);
  updateLCDStatus(currentDisplayState, acRelayState);
}

void displayTime(const DateTime& now) {
  char timeBuffer[17];
  snprintf(timeBuffer, sizeof(timeBuffer), "Time:%02d:%02d:%02d", now.hour(), now.minute(), now.second());
  lcd.setCursor(0, 0);
  lcd.print(timeBuffer);
}

void updateLCDStatus(DisplayState state, bool acOn) {
  String status;
  switch (state) {
    case DISPLAY_MANUAL: status = "Manual Override "; break;
    case DISPLAY_BLYNK:  status = "Blynk Control ON"; break;
    case DISPLAY_MOTION: status = "Motion Detected "; break;
    case DISPLAY_NO_MOTION: status = "No Motion      "; break;
    case DISPLAY_BRIGHT: status = "Room is Bright "; break;
  }
  if (acOn) status = "AC ON | " + status;
  if (status != lastLCDMessage) {
    lcd.setCursor(0, 1);
    lcd.print(status);
    lastLCDMessage = status;
  }
}

// Temperature reading with fallback simulation
float readTemperature() {
  float temp = dhtSensor.getTemperature();
  if (isnan(temp)) {
    static float sim = 24.0;
    sim += 0.1;
    if (sim > 30.0) sim = 24.0;
    return sim;
  }
  return temp;
}
