# Smart Room Simulation Overview

## 1. Wokwi Simulation
- File: `smart_room.wokwi.json`
- URL: [Open in Wokwi](//wokwi.com/projects/438086795266341889)
- Simulates ESP32 with PIR, LDR, DHT22, RTC, and relays.
- Blynk cloud is mocked via serial console or replaced by button toggles.

## 2. Proteus Simulation
- Files: `smart_room.pdsprj`, `smart_room.dsn`
- Includes LCD, Relay modules, and real-time behavior.
- Open using Proteus 8.13 or higher.

## Instructions
- Set DHT22 data pin to digital pin 15 (as used in code).
- Simulate relay switching based on sensor thresholds.
- Observe LCD output and light control based on PIR + LDR logic.
