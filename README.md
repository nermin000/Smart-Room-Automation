Smart Room Automation System using ESP32 and Blynk IoT
======================================================

Overview
--------

This project demonstrates a sophisticated **Smart Room Automation System** leveraging the ESP32 microcontroller, Blynk IoT platform, and a suite of environmental sensors. It integrates real-time environmental monitoring and intelligent control logic to enhance comfort, energy efficiency, and remote management in residential or office spaces.

Key Features
------------

*   **Motion-Activated Lighting:** Utilizes a PIR sensor to detect presence and automatically control room lighting, enhancing convenience and power savings.
    
*   **Ambient Brightness Sensing:** Employs an LDR sensor to evaluate natural light levels, avoiding unnecessary artificial lighting. LDR threshold is stored in EEPROM for persistence.
    
*   **Temperature-Based AC Control:** Integrates DHT22 sensor readings to manage air conditioning via relay, with configurable hysteresis for smooth operation.
    
*   **Real-Time Clock Scheduling:** Uses DS3231 RTC to implement sleep modes during designated off-hours, optimizing power consumption.
    
*   **EEPROM-Based Configuration:** Stores user-defined parameters like LDR threshold and sleep schedule, preserving settings across reboots.
    
*   **Dual Control Interface:** Supports manual override via physical switch and remote control through the Blynk mobile app, allowing flexible user interaction.
    
*   **Status Display:** Provides real-time feedback on system states and current time on an I2C LCD display.
    
*   **Power-Efficient Operation:** Implements ESP32 light sleep during off-hours to reduce energy usage without sacrificing responsiveness.
    
*   **Task-Based Architecture:** Utilizes TaskScheduler for concurrent, non-blocking task management.
    

Hardware Components
-------------------

ComponentDescriptionConnected Pin (ESP32)ESP32 Dev BoardCentral controller—PIR Motion SensorDetects room occupancyGPIO 34LDR SensorMeasures ambient light intensityGPIO 35 (Analog)DHT22 SensorMeasures temperature & humidityGPIO 32Relay Module (AC)Controls Air ConditionerGPIO 18Relay Module (Light)Controls lightingGPIO 19Physical SwitchManual override for lightingGPIO 23LCD Display (I2C)Shows status & timeSDA/SCL (I2C)DS3231 RTC ModuleReal-time clock for schedulingSDA/SCL (I2C)

Software Architecture
---------------------

*   **TaskScheduler Library:** Implements a non-blocking multitasking environment for sensor updates, Blynk communication, output control, and sleep management.
    
*   **Blynk IoT Platform:** Provides cloud-based remote monitoring and manual override capabilities via smartphone.
    
*   **EEPROM Storage:** Retains LDR threshold and other critical user settings across reboots.
    
*   **Interrupt-Driven Motion Detection:** Ensures immediate response to occupancy changes without blocking main execution.
    
*   **Hysteresis-Based AC Control:** Prevents relay chatter by defining upper and lower temperature thresholds.
    
*   **Sleep Mode Management:** Automatically triggers ESP32 light sleep during late night and early morning hours, preserving system responsiveness with minimal power draw.
    

Setup Instructions
------------------

1.  **Hardware Assembly:** Connect all sensors, relays, switch, and LCD to the ESP32 as per the pin configuration above.
    
2.  **Software Environment:** Use Arduino IDE with ESP32 board support installed.
    
3.  **Library Dependencies:** Ensure the following libraries are installed:
    
    *   Blynk
        
    *   TaskScheduler
        
    *   DHTesp or DHT
        
    *   RTClib
        
    *   LiquidCrystal\_I2C
        
    *   EEPROM
        
4.  **Blynk Configuration:**
    
    *   Replace the BLYNK\_AUTH\_TOKEN in the code with your own from the Blynk app.
        
    *   Set your WiFi credentials (ssid and pass).
        
5.  **Upload the Sketch:** Compile and upload the code to the ESP32.
    
6.  **Calibration & Testing:**
    
    *   Verify EEPROM values are correctly loaded/stored (e.g., LDR threshold).
        
    *   Confirm sensor readings and relay switching.
        
    *   Test manual override via the switch and Blynk app.
        
    *   Check LCD output and scheduled sleep behavior.
        

Usage & Control
---------------

*   **Automatic Mode:** System autonomously controls lighting and AC based on motion, light, and temperature sensors.
    
*   **Manual Override:** Physical switch or Blynk app buttons can override automatic control logic for lights and AC.
    
*   **Status Monitoring:** LCD displays current time, temperature, humidity, light levels, and relay status for quick local feedback.
    
*   **Power Saving:** System enters light sleep during predefined off-hours to conserve energy without requiring full shutdown.
    

EEPROM-Enhanced Configurations
------------------------------

VariableStored In EEPROMPurposeLDR ThresholdYesDefine the ambient light level for triggering lightsSleep ScheduleOptionalFuture expansion for storing sleep start/end hours

EEPROM ensures critical thresholds remain persistent across reboots, enabling long-term deployment.

Future Enhancements
-------------------

*   **Web-Based Dashboard:** Develop a responsive web interface hosted on the ESP32 for richer control and analytics.
    
*   **Dynamic Scheduling:** Add ability to configure sleep time and thresholds via Blynk or web.
    
*   **Data Logging:** Store temperature, humidity, and occupancy trends in SPIFFS or SD card.
    
*   **Machine Learning:** Introduce pattern recognition for predictive control and adaptive automation.
    
*   **Security Features:** Add TLS encryption for secure communication with Blynk/cloud services.
    

Author
------

**Narmin Othman** — Freshly graduated electrical and computer engineer passionate about building intelligent embedded systems that promote sustainable and comfortable living environments.

License
-------

This project is open-source under the MIT License — feel free to use, modify, and expand it with attribution.