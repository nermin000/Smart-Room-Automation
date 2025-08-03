
# Smart Room Automation System using ESP32 and Blynk IoT

## Overview

This project demonstrates a sophisticated **Smart Room Automation System** leveraging the ESP32 microcontroller, Blynk IoT platform, and a suite of environmental sensors. It integrates real-time environmental monitoring and intelligent control logic to enhance comfort, energy efficiency, and remote management in residential or office spaces.

---

## Key Features

- **Motion-Activated Lighting:** Utilizes a PIR sensor to detect presence and automatically control room lighting, enhancing convenience and power savings.
- **Ambient Brightness Sensing:** Employs an LDR sensor to evaluate natural light levels, avoiding unnecessary artificial lighting.
- **Temperature-Based AC Control:** Integrates DHT22 sensor readings to manage air conditioning via relay, with configurable hysteresis for smooth operation.
- **Real-Time Clock Scheduling:** Uses DS3231 RTC to implement sleep modes during designated off-hours, optimizing power consumption.
- **Dual Control Interface:** Supports manual override via physical switch and remote control through the Blynk mobile app, allowing flexible user interaction.
- **Status Display:** Provides real-time feedback on system states and current time on an I2C LCD display.
- **Power-Efficient Operation:** Implements ESP32 light sleep during off-hours to reduce energy usage without sacrificing responsiveness.

---

## Hardware Components

| Component            | Description                      | Connected Pin (ESP32) |
|----------------------|---------------------------------|----------------------|
| ESP32 Dev Board      | Central controller               | —                    |
| PIR Motion Sensor     | Detects room occupancy           | GPIO 34              |
| LDR Sensor            | Measures ambient light intensity | GPIO 35 (Analog)     |
| DHT22 Sensor          | Measures temperature & humidity  | GPIO 32              |
| Relay Module          | Controls Air Conditioner (AC)    | GPIO 18              |
| LED (Light Control)   | Indicates lighting status        | GPIO 19              |
| Physical Switch       | Manual override for lighting     | GPIO 23              |
| LCD Display (I2C)     | Shows status & time              | SDA/SCL (I2C)        |
| DS3231 RTC Module     | Real-time clock for scheduling   | SDA/SCL (I2C)        |

---

## Software Architecture

- **TaskScheduler Library:** Implements a non-blocking multitasking environment for sensor updates, Blynk communication, output control, and sleep management.
- **Blynk IoT Platform:** Provides cloud-based remote monitoring and manual override capabilities via smartphone.
- **Interrupt-Driven Motion Detection:** Ensures immediate response to occupancy changes without blocking main execution.
- **Hysteresis-Based AC Control:** Prevents relay chatter by defining upper and lower temperature thresholds.
- **Sleep Mode Management:** Automatically triggers ESP32 light sleep during late night and early morning hours, preserving system responsiveness with minimal power draw.

---

## Setup Instructions

1. **Hardware Assembly:** Connect all sensors, relay, switch, and LCD to the ESP32 as per the pin configuration above.
2. **Software Environment:** Use Arduino IDE with ESP32 board support installed.
3. **Library Dependencies:** Ensure the following libraries are installed:
   - `Blynk`
   - `TaskScheduler`
   - `DHTesp`
   - `RTClib`
   - `LiquidCrystal_I2C`
4. **Blynk Configuration:** 
   - Replace the `BLYNK_AUTH_TOKEN` in the code with your own from the Blynk app.
   - Set your WiFi credentials (`ssid` and `pass`).
5. **Upload the Sketch:** Compile and upload the code to the ESP32.
6. **Calibration & Testing:** 
   - Confirm sensor readings and relay switching.
   - Test manual switch and Blynk control.
   - Verify LCD output and sleep behavior.

---

## Usage & Control

- **Automatic Mode:** System autonomously controls lighting and AC based on sensor inputs.
- **Manual Override:** Physical switch or Blynk app buttons can override automatic control for lights and AC.
- **Status Monitoring:** LCD displays current time and system state for quick local feedback.
- **Power Saving:** System enters light sleep during predefined off-hours to conserve energy.

---

## Future Enhancements

- **Web-Based Dashboard**: Develop a custom web interface hosted on the ESP32 or cloud for richer user experience.
- **Additional Sensors:** Integrate air quality, humidity, or occupancy sensors for holistic environment control.
- **Machine Learning:** Implement adaptive algorithms to learn user habits and optimize system behavior dynamically.
- **Security Features:** Add authentication layers and encrypted communication for enhanced IoT safety.

---

## Author

**Narmin Othman** — Freshly graduated electrical and computer engineer with a passion for developing technology that enhances human well-being and sustainable living.

---

## License

This project is open-source under the MIT License — feel free to use, modify, and expand it with attribution.
