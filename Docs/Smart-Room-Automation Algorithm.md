# Smart Lighting & AC Control System

This document outlines the structured algorithm for a smart system that automates lighting and air conditioning based on environmental conditions, manual input, and remote control via Blynk IoT platform.

---

## Overview

The system performs the following tasks:

- Reads motion, brightness, and temperature sensors.
- Controls LED lighting and AC relay output.
- Displays system status on an LCD.
- Accepts control input from a physical switch and Blynk mobile app.
- Stores settings persistently using EEPROM with CRC checking.
- Saves power by entering light sleep mode during inactive hours.

---

## Step-by-Step Algorithm

### Step 1: Initialization

1. Begin EEPROM and load stored settings.  
2. If settings are invalid or corrupted:  
   - Load default values.  
   - Save valid settings back to EEPROM with a CRC.  
3. Connect to Wi-Fi and Blynk using credentials.  
4. Initialize all hardware:  
   - Sensors (PIR, LDR, DHT22)  
   - LCD display  
   - RTC (Real-Time Clock)  
   - GPIO pins  
5. If RTC lost power, reset it to a default time.  
6. Apply initial settings to LED and AC.  
7. Start task scheduler and enable tasks.  

---

### Step 2: Main Loop

- Continuously run all scheduled tasks:  
  - Blynk communication  
  - Sensor updates  
  - Output control  
  - Sleep check  

---

### Step 3: Read Inputs

- Read motion sensor (PIR).  
- Read brightness from LDR.  
- Read temperature from DHT22.  
- Check manual switch (pressed or not).  
- Get current time from RTC.  

---

### Step 4: LED Control

- Turn ON LED if any of the following is true:  
  - Blynk light override is active.  
  - Manual switch is ON.  
  - Room is dark and motion is detected.  
- Otherwise, turn OFF the LED.  

---

### Step 5: AC Control

- If Blynk AC override is ON:  
  - Turn ON the AC relay.  
- Else:  
  - If motion is detected and temperature > HIGH threshold:  
    - Turn ON AC.  
  - If temperature < LOW threshold or no motion:  
    - Turn OFF AC.  

---

### Step 6: Update LCD

- Line 1: Display current time (HH:MM:SS).  
- Line 2: Show system status message:  
  - "Manual Override"  
  - "Blynk Control ON"  
  - "Motion Detected"  
  - "No Motion"  
  - "Room is Bright"  
- If AC is ON, prepend the message with "AC ON | ".  

---

### Step 7: Save Blynk Changes

- When Blynk virtual pins change:  
  - Update settings (LED or AC override).  
  - Save to EEPROM with updated CRC.  

---

### Step 8: Power Saving (Sleep Mode)

- If current time is before 6:00 AM or after 11:00 PM:  
  - Enable timer wake-up.  
  - Enter light sleep for a few seconds.  
  - Automatically wake and continue.  

---

## Notes

- CRC32 ensures safe and reliable EEPROM data.  
- TaskScheduler allows smooth handling of timed tasks.  
- Blynk provides real-time control via mobile app.  
- Light sleep helps reduce power usage during the night.
