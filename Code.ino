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
#include <EEPROM.h>
#include <Arduino.h>  // for crc32 if needed

// ==== [WiFi Credentials] ====
const char ssid[] = "Wokwi-GUEST";
const char pass[] = "";

// ==== [Pin Configuration] ====
constexpr int PIN_DHT22 = 32;
constexpr int PIN_PIR = 34;
constexpr int PIN_LDR = 35;
constexpr int PIN_LED = 19;
constexpr int PIN_SWITCH = 23;
constexpr int PIN_RELAY_AC = 18;

// ==== [Constants] ====
constexpr int BRIGHTNESS_THRESHOLD = 170;
constexpr float TEMP_HIGH = 28.0;
constexpr float TEMP_LOW = 24.0;
constexpr uint64_t SLEEP_DURATION_US = 3000000ULL; // 3 seconds

// ==== [EEPROM Config] ====
constexpr int EEPROM_SIZE = 64;
constexpr uint8_t EEPROM_VERSION = 1;
constexpr int EEPROM_ADDR_SETTINGS = 0;

// ==== [Persistent Settings Struct] ====
struct Settings {
  uint8_t version;
  bool blynkLightOverride;
  bool blynkACOverride;
  uint32_t crc32; // checksum for data integrity

  // Compute CRC32 of struct except crc32 itself
  uint32_t computeCRC() const {
    // Simple crc32 implementation (Arduino built-in CRC32 can be used)
    const uint8_t* data = reinterpret_cast<const uint8_t*>(this);
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < sizeof(Settings) - sizeof(crc32); ++i) {
      crc ^= data[i];
      for (int j = 0; j < 8; j++) {
        if (crc & 1) crc = (crc >> 1) ^ 0xEDB88320;
        else crc >>= 1;
      }
    }
    return ~crc;
  }
};

// ==== [Global Variables] ====
Settings settings;
LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS3231 rtc;
DHTesp dhtSensor;

bool motionDetected = false;
bool acRelayState = false;
enum DisplayState { DISPLAY_MANUAL, DISPLAY_BLYNK, DISPLAY_MOTION, DISPLAY_NO_MOTION, DISPLAY_BRIGHT };
DisplayState currentDisplayState = DISPLAY_NO_MOTION;
String lastLCDMessage = "";

// ==== [Task Prototypes] ====
void taskBlynkRun();
void taskUpdateSensors();
void taskUpdateOutputs();
void taskSleepCheck();

// ==== [TaskScheduler Setup] ====
Scheduler runner;
Task taskBlynk(10, TASK_FOREVER, taskBlynkRun);
Task taskSensors(1000, TASK_FOREVER, taskUpdateSensors);
Task taskOutputs(500, TASK_FOREVER, taskUpdateOutputs);
Task taskSleep(60000, TASK_FOREVER, taskSleepCheck);

// ==== [Helper Function Prototypes] ====
void loadSettings();
void saveSettings();
void applySettings();
void updateLED();
void updateAC(float temperature);
void updateLCD(const DateTime& now);
void displayTime(const DateTime& now);

// ==== [Blynk Virtual Pin Handlers] ====
BLYNK_WRITE(V0) {
  bool val = param.asInt();
  if (settings.blynkLightOverride != val) {
    settings.blynkLightOverride = val;
    saveSettings();
  }
}

BLYNK_WRITE(V1) {
  bool val = param.asInt();
  if (settings.blynkACOverride != val) {
    settings.blynkACOverride = val;
    if (!val) {
      acRelayState = false;
      digitalWrite(PIN_RELAY_AC, HIGH);
    }
    saveSettings();
  }
}

// ==== [Setup] ====
void setup() {
  EEPROM.begin(EEPROM_SIZE);
  loadSettings();

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  pinMode(PIN_PIR, INPUT);
  pinMode(PIN_DHT22, INPUT);
  pinMode(PIN_LDR, INPUT);
  pinMode(PIN_SWITCH, INPUT_PULLUP);
  pinMode(PIN_RELAY_AC, OUTPUT);
  pinMode(PIN_LED, OUTPUT);

  digitalWrite(PIN_RELAY_AC, HIGH); // Relay OFF initially

  dhtSensor.setup(PIN_DHT22, DHTesp::DHT22);
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

  applySettings();

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
  runner.execute();
}

// ==== [Task Implementations] ====
void taskBlynkRun() {
  Blynk.run();
}

void taskUpdateSensors() {
  motionDetected = (digitalRead(PIN_PIR) == HIGH);
}

void taskUpdateOutputs() {
  DateTime now = rtc.now();
  bool manualOverride = (digitalRead(PIN_SWITCH) == LOW);
  bool isBright = (analogRead(PIN_LDR) < BRIGHTNESS_THRESHOLD);
  float temperature = dhtSensor.getTemperature();

  // Update LED
  updateLED();

  // Update AC
  if (settings.blynkACOverride) {
    digitalWrite(PIN_RELAY_AC, LOW);
    acRelayState = true;
  } else {
    updateAC(temperature);
  }

  // Determine display state
  if (manualOverride) currentDisplayState = DISPLAY_MANUAL;
  else if (settings.blynkLightOverride) currentDisplayState = DISPLAY_BLYNK;
  else if (motionDetected) currentDisplayState = DISPLAY_MOTION;
  else if (isBright) currentDisplayState = DISPLAY_BRIGHT;
  else currentDisplayState = DISPLAY_NO_MOTION;

  updateLCD(now);
}

void taskSleepCheck() {
  DateTime now = rtc.now();
  if (now.hour() < 6 || now.hour() > 23) {
    esp_sleep_enable_timer_wakeup(SLEEP_DURATION_US);
    esp_light_sleep_start();
  }
}

// ==== [Helper Functions] ====
void loadSettings() {
  EEPROM.get(EEPROM_ADDR_SETTINGS, settings);
  if (settings.version != EEPROM_VERSION || settings.computeCRC() != settings.crc32) {
    // Invalid or uninitialized EEPROM, load defaults
    settings.version = EEPROM_VERSION;
    settings.blynkLightOverride = false;
    settings.blynkACOverride = false;
    saveSettings();
  }
}

void saveSettings() {
  settings.crc32 = settings.computeCRC();
  EEPROM.put(EEPROM_ADDR_SETTINGS, settings);
  EEPROM.commit();
}

void applySettings() {
  if (settings.blynkLightOverride) digitalWrite(PIN_LED, HIGH);
  else digitalWrite(PIN_LED, LOW);

  if (settings.blynkACOverride) {
    digitalWrite(PIN_RELAY_AC, LOW);
    acRelayState = true;
  } else {
    digitalWrite(PIN_RELAY_AC, HIGH);
    acRelayState = false;
  }
}

void updateLED() {
  bool manualOverride = (digitalRead(PIN_SWITCH) == LOW);
  bool isBright = (analogRead(PIN_LDR) < BRIGHTNESS_THRESHOLD);

  if (settings.blynkLightOverride || manualOverride || (!isBright && motionDetected)) {
    digitalWrite(PIN_LED, HIGH);
  } else {
    digitalWrite(PIN_LED, LOW);
  }
}

void updateAC(float temperature) {
  if (motionDetected && !acRelayState && temperature > TEMP_HIGH) {
    acRelayState = true;
    digitalWrite(PIN_RELAY_AC, LOW);
  } else if (acRelayState && (temperature < TEMP_LOW || !motionDetected)) {
    acRelayState = false;
    digitalWrite(PIN_RELAY_AC, HIGH);
  }
}

void updateLCD(const DateTime& now) {
  displayTime(now);

  String status;
  switch (currentDisplayState) {
    case DISPLAY_MANUAL: status = "Manual Override "; break;
    case DISPLAY_BLYNK: status = "Blynk Control ON"; break;
    case DISPLAY_MOTION: status = "Motion Detected "; break;
    case DISPLAY_NO_MOTION: status = "No Motion      "; break;
    case DISPLAY_BRIGHT: status = "Room is Bright "; break;
  }
  if (acRelayState) status = "AC ON | " + status;

  if (status != lastLCDMessage) {
    lcd.setCursor(0, 1);
    lcd.print(status);
    lastLCDMessage = status;
  }
}

void displayTime(const DateTime& now) {
  char timeBuffer[17];
  snprintf(timeBuffer, sizeof(timeBuffer), "Time:%02d:%02d:%02d", now.hour(), now.minute(), now.second());
  lcd.setCursor(0, 0);
  lcd.print(timeBuffer);
}

