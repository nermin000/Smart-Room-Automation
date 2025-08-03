# Smart Room Automation System using ESP32 and Blynk

This project demonstrates a Smart Room Automation System based on ESP32 that integrates sensor-based decision-making and IoT remote control using the Blynk platform.

## 🌟 Features

- **Motion Detection** using PIR sensor to control lights.
- **Brightness Control** with LDR for ambient light adjustment.
- **Temperature-based AC Control** using DHT22.
- **RTC (DS3231)** to trigger power-saving modes during inactive hours.
- **Manual Switch** for local control.
- **LCD Feedback** for real-time system status.
- **EEPROM** support for storing persistent settings.
- **Scheduled Task Management** with `TaskScheduler`.

## 🧰 Components Used

| Component       | Description                      |
|----------------|----------------------------------|
| ESP32           | Main microcontroller              |
| PIR Sensor       | Motion detection                  |
| LDR             | Light sensor                      |
| DHT22           | Temperature and humidity sensor   |
| DS3231 RTC      | Real-time clock                   |
| Relay Module    | Control AC/LED                    |
| LCD 16x2 + I2C  | Display system info               |
| Blynk Platform  | IoT control & monitoring          |
| EEPROM (built-in) | Save persistent settings        |

## 🧠 Logic Overview

1. **Motion + Light**: If motion is detected and the room is dark, the light turns on.
2. **Temperature Control**: AC turns on when the temperature exceeds 28°C and turns off below 24°C.
3. **Sleep Mode**: Between 12 AM and 6 AM, the system disables motion/light automation.
4. **Manual Override**: Local physical switch or Blynk button can override automation.
5. **Scheduled Tasking**: Tasks like updating Blynk, checking sensors, outputs, and sleep are handled via a task scheduler.

## 📂 Project Structure

```plaintext
Smart-Room-Automation/
├── code/           ← Arduino source code (.ino)
├── docs/           ← Architecture diagrams, flowcharts
├── images/         ← System screenshots, LCD states
├── simulations/    ← Wokwi/Proteus files (optional)
├── README.md       ← Project overview and details
```

## 🛠️ Libraries Required

- `WiFi.h`
- `BlynkSimpleEsp32.h`
- `DHT.h`
- `RTClib.h`
- `LiquidCrystal_I2C.h`
- `TaskScheduler.h`
- `EEPROM.h`

## 💡 Future Enhancements

- Add web-based UI (ESP32 Web Server)
- Add auto-dimming LED control via PWM
- Integration with voice assistants (Google/Alexa)
- Power usage statistics and energy optimization

---

Created with ❤️ by Nermin | 2025