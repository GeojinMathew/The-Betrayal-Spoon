<img width="1280" height="640" alt="git (1)" src="https://github.com/user-attachments/assets/8920b256-2ba8-4988-b824-5351134eb4bd" />

# The Betrayal Spoon 🥄💀

## Basic Details

### Team Name: Vivet

### Team Members

- Member 1: Geojin Mathew - Saintgits College of Engineering
- Member 2: Meeval Varghese - Saintgits College of Engineering

### Project Description

The Betrayal Spoon is an intentionally useless smart spoon that detects when it approaches the user's mouth and randomly decides whether to spill the food, do nothing, or fake a spill.

Built using an ESP32, ultrasonic sensor, servo motor, and audio module, it turns a simple eating experience into an unpredictable one.

---

### The Problem (that doesn't exist)

People have it too easy while eating.

Normal spoons are predictable, reliable, and boring. There is absolutely no reason for a spoon to betray you — so naturally, we decided to build one that does.

---

### The Solution (that nobody asked for)

The Betrayal Spoon uses an HC-SR04 ultrasonic sensor to detect when the spoon is close to the user's mouth.

Once the distance reaches approximately 10 cm, the ESP32 randomly chooses one of three outcomes:

- 💀 **Complete Spill** — The spoon rotates and spills the food.
- 😐 **No Spill** — The spoon does nothing and behaves normally.
- 😈 **Fake Spill** — The spoon makes a small movement to make the user think it will spill, but doesn't actually spill.

A corresponding meme sound is also played through the speaker depending on the selected outcome.

---

## Technical Details

### Technologies/Components Used

### For Software:

- Arduino C/C++
- Arduino IDE
- ESP32 Arduino Core
- ESP32Servo Library
- UART communication
- FN-M16P MP3 command protocol

### For Hardware:

- ESP32
- HC-SR04 Ultrasonic Sensor
- SG90 Servo Motor
- FN-M16P MP3 Audio Module
- 8Ω Speaker
- MicroSD Card
- Spoon
- Custom handheld enclosure/mechanical assembly
- Resistors
- Jumper wires
- 5V power supply

---

### Pin Configuration

#### HC-SR04 → ESP32

| HC-SR04 | ESP32 |
|---|---|
| VCC | 5V |
| GND | GND |
| TRIG | GPIO 5 |
| ECHO | GPIO 18 |

The ECHO signal is connected through a voltage divider:

```text
HC-SR04 ECHO
     │
    1kΩ
     │
     ├──────── GPIO 18
     │
    2kΩ
     │
    GND
