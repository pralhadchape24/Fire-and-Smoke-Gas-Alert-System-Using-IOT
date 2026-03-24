# 🔥 PYROS — IoT Fire & Gas Safety Monitoring System

![Status](https://img.shields.io/badge/Status-Active-brightgreen)
![Platform](https://img.shields.io/badge/Platform-ESP32-blue)
![Language](https://img.shields.io/badge/Language-C%2B%2B%20%7C%20HTML%2FJS-orange)
![License](https://img.shields.io/badge/License-MIT-purple)

> A real-time IoT-based Fire and Gas Safety Monitoring System using ESP32, MQ2 gas sensor, flame sensor, buzzer, LED, and servo motor — with a live web dashboard served directly from the ESP32 over local WiFi. No Firebase. No Blynk. No internet required.

---

## 📸 Dashboard Preview

```
┌─────────────────────────────────────────────────────────┐
│  🔥 PYROS  Fire & Gas Safety Command          LIVE ●   │
├──────────────┬──────────────────────┬───────────────────┤
│ GAS LEVEL    │   ⬡ STATUS CORE      │  🕯️ FLAME SENSOR  │
│ [Thermometer]│   [Reactor Display]  │  [Orb Animation]  │
│              │                      │                   │
│ SERVO VALVE  │   [Segment Display]  │  SYSTEM HEALTH    │
│ [Dial Gauge] │   Gas ADC Reading    │  [Status Grid]    │
├──────────────┴──────────────────────┴───────────────────┤
│              📈 Gas Waveform — Real-time Chart           │
├─────────────────────────────────────────────────────────┤
│  Total | Peak Gas | Alert Events | Uptime               │
├─────────────────────────────────────────────────────────┤
│              📋 Alert History Log (last 10)              │
└─────────────────────────────────────────────────────────┘
```

---

## 📋 Table of Contents

- [Features](#-features)
- [Components](#-components)
- [Circuit Connections](#-circuit-connections)
- [Pin Reference](#-pin-reference)
- [System Architecture](#-system-architecture)
- [Alert Logic](#-alert-logic)
- [Installation](#-installation)
- [Libraries Required](#-libraries-required)
- [How to Run](#-how-to-run)
- [Dashboard Features](#-dashboard-features)
- [API Endpoint](#-api-endpoint)
- [File Structure](#-file-structure)
- [Troubleshooting](#-troubleshooting)

---

## ✨ Features

### Hardware Features
- 🔴 **Dual-mode MQ2 sensing** — reads both Analog (intensity) and Digital (threshold) outputs
- 🔴 **Dual-mode Flame sensing** — reads both Analog (intensity) and Digital (trigger) outputs
- 🔔 **Active Buzzer alert** — sounds immediately on gas or fire detection
- 💡 **LED indicator** — visual alert on detection
- ⚙️ **Servo motor control** — automatically opens vent/door on alert (90°), closes on safe (0°)
- ⏱️ **3-minute MQ2 warm-up** — sensor readings ignored until MQ2 heater stabilizes
- 🔁 **Auto-reset alerts** — alert automatically resets after 3 seconds if condition clears
- 📡 **Built-in HTTP WebServer** — ESP32 serves live sensor data as JSON over local WiFi
- 🌐 **No cloud dependency** — works fully offline on local network

### Dashboard Features
- 🌡️ **Live thermometer bar** — animated vertical fill showing gas level with heat gradient
- ⬡ **Reactor core status display** — pulsing animated ring with status emoji
- 📊 **Segmented digit display** — large glowing gas ADC reading
- 🔥 **Flame orb animation** — pulsing rings when flame detected
- ⚙️ **Servo dial gauge** — SVG needle showing valve angle in real time
- 📈 **Real-time waveform chart** — scrolling gas level graph with threshold line
- 🕰️ **Alert history log** — last 10 readings with timestamp, gas value, flame, status
- 🔢 **Session stats** — total readings, peak gas, alert count, uptime timer
- 🌋 **Fire simulation background** — cellular automaton fire renders behind UI on CRITICAL
- 🫧 **Lava lamp bubbles** — speed and color change with danger level
- ⚠️ **Animated hazard stripe** — diagonal warning tape on WARNING/CRITICAL
- 📺 **CRT scanline overlay** — industrial terminal aesthetic
- 🖥️ **Screen shake** — full UI shakes on CRITICAL status
- 🔴 **Red danger border** — glowing frame pulses on CRITICAL
- 🎨 **Dynamic theming** — background color shifts based on SAFE / WARNING / CRITICAL

---

## 🧰 Components

| # | Component | Specification | Quantity |
|---|-----------|--------------|----------|
| 1 | **ESP32 Development Board** | ESP32-WROOM-32 / ESP32 DevKit V1 | 1 |
| 2 | **MQ2 Gas Sensor** | Detects LPG, smoke, alcohol, propane, hydrogen | 1 |
| 3 | **Flame Sensor Module** | IR-based, digital + analog output, 760nm–1100nm | 1 |
| 4 | **Active Buzzer** | 5V, 85dB | 1 |
| 5 | **LED** | Red, 5mm | 1 |
| 6 | **Resistor** | 220Ω (for LED) | 1 |
| 7 | **Servo Motor** | SG90 (5V, 180°) | 1 |
| 8 | **Breadboard** | 830 tie-point | 1 |
| 9 | **Jumper Wires** | Male-to-Male, Male-to-Female | As needed |
| 10 | **USB Cable** | Micro USB / USB-C (for ESP32) | 1 |
| 11 | **Power Supply** | 5V via USB or external | 1 |

---

## 🔌 Circuit Connections

### MQ2 Gas Sensor → ESP32
```
MQ2 Pin        ESP32 Pin       Notes
─────────────────────────────────────────────
VCC       →    3.3V or 5V      Use 5V for better sensitivity
GND       →    GND
AO        →    GPIO 36         Analog output (reads gas intensity)
DO        →    GPIO 34         Digital output (HIGH/LOW threshold)
```
> ⚠️ GPIO 36 and 39 are **input-only** on ESP32 — do NOT connect pull-up/down resistors.

---

### Flame Sensor Module → ESP32
```
Flame Pin      ESP32 Pin       Notes
─────────────────────────────────────────────
VCC       →    3.3V
GND       →    GND
AO        →    GPIO 39         Analog output (flame intensity)
DO        →    GPIO 35         Digital output (LOW = flame detected)
```
> ℹ️ Most flame sensor modules output LOW when flame is detected (active-low logic).

---

### Active Buzzer → ESP32
```
Buzzer Pin     ESP32 Pin       Notes
─────────────────────────────────────────────
+  (VCC)  →    GPIO 25         Drive directly from GPIO
-  (GND)  →    GND
```

---

### LED → ESP32
```
LED Pin        ESP32 Pin       Notes
─────────────────────────────────────────────
Anode (+) →    GPIO 26         Via 220Ω resistor in series
Cathode (-) →  GND
```

---

### Servo Motor → ESP32
```
Servo Pin      ESP32 Pin       Notes
─────────────────────────────────────────────
Signal    →    GPIO 13         PWM signal
VCC       →    5V (external)   Do NOT use ESP32 3.3V — servo needs 5V
GND       →    GND (common)    Share GND with ESP32
```
> ⚠️ Power the servo from a separate 5V supply or VUSB pin. Drawing from ESP32 3.3V will cause brownout resets.

---

## 📌 Pin Reference

| Pin | GPIO | Direction | Component | Signal Type |
|-----|------|-----------|-----------|-------------|
| MQ2_A | 36 | INPUT | MQ2 Gas Sensor | Analog (0–4095) |
| MQ2_D | 34 | INPUT | MQ2 Gas Sensor | Digital (0 or 1) |
| FLAME_A | 39 | INPUT | Flame Sensor | Analog (0–4095) |
| FLAME_D | 35 | INPUT | Flame Sensor | Digital (0=flame) |
| BUZZER | 25 | OUTPUT | Active Buzzer | Digital HIGH/LOW |
| LED_PIN | 26 | OUTPUT | Red LED | Digital HIGH/LOW |
| SERVO_PIN | 13 | OUTPUT | SG90 Servo | PWM |

---

## 🏗️ System Architecture

```
┌──────────────────────────────────────────────────────┐
│                    ESP32 DevKit                       │
│                                                      │
│  ┌──────────┐    ┌──────────────────────────────┐   │
│  │  MQ2     │    │         FIRMWARE              │   │
│  │  GPIO36  │───▶│  readSensors()  every 2s      │   │
│  │  GPIO34  │───▶│  checkSensors() — logic        │   │
│  └──────────┘    │  triggerAlert() — actuators   │   │
│                  │  resetAlert()  — auto-reset    │   │
│  ┌──────────┐    │                               │   │
│  │ Flame    │    │  ┌─────────────────────────┐  │   │
│  │  GPIO39  │───▶│  │   HTTP WebServer :80    │  │   │
│  │  GPIO35  │───▶│  │   GET /      → info    │  │   │
│  └──────────┘    │  │   GET /data  → JSON    │  │   │
│                  │  └─────────────────────────┘  │   │
│  ┌──────────┐    └──────────────────────────────┘   │
│  │ Buzzer   │◀── GPIO 25 (OUTPUT)                   │
│  │ LED      │◀── GPIO 26 (OUTPUT)                   │
│  │ Servo    │◀── GPIO 13 (PWM)                      │
│  └──────────┘                                        │
└──────────────────────────────────────────────────────┘
              │ WiFi (2.4GHz)
              ▼
┌──────────────────────────────────────────────────────┐
│              Browser (same WiFi network)              │
│                                                      │
│   pyros_dashboard.html                               │
│   fetch("http://192.168.x.x/data") every 2s         │
│                                                      │
│   Displays: Gas level, Flame status, Status,         │
│   Chart, Servo angle, History log, Stats             │
└──────────────────────────────────────────────────────┘
```

---

## 🚦 Alert Logic

```
┌─────────────────────────────────────────────────────┐
│              SENSOR READING (every 2s)              │
│                                                     │
│  smokeAnalog  = analogRead(GPIO 36)                 │
│  smokeDigital = digitalRead(GPIO 34)                │
│  flameAnalog  = analogRead(GPIO 39)                 │
│  flameDigital = digitalRead(GPIO 35)                │
└───────────────────────┬─────────────────────────────┘
                        │
          ┌─────────────▼──────────────┐
          │  smokeAnalog > 500  ?       │
          │  AND                        │
          │  (flameAnalog < 500 OR      │
          │   flameDigital == LOW) ?    │
          └──────┬────────────┬─────────┘
                 │YES         │NO
                 ▼            ▼
          ┌──────────┐  ┌──────────────────────────┐
          │ CRITICAL │  │  smokeAnalog > 500  ?    │
          │ GAS+FIRE │  └──────┬───────────┬────────┘
          └──────────┘        │YES        │NO
                              ▼           ▼
                        ┌─────────┐ ┌──────────────────────────┐
                        │ WARNING │ │ flameAnalog < 500 OR     │
                        │  GAS    │ │ flameDigital == LOW ?    │
                        └─────────┘ └──────┬───────────┬────────┘
                                          │YES        │NO
                                          ▼           ▼
                                    ┌─────────┐ ┌────────┐
                                    │CRITICAL │ │  SAFE  │
                                    │  FIRE   │ │        │
                                    └─────────┘ └────────┘
```

### Actuator Response Table

| Status | Buzzer | LED | Servo | Dashboard |
|--------|--------|-----|-------|-----------|
| **SAFE** | OFF | OFF | 0° (Closed) | Green theme |
| **WARNING** | ON | ON | 90° (Partial) | Amber theme + hazard stripe |
| **CRITICAL** | ON | ON | 90° (Open) | Red theme + fire BG + shake |

> Auto-reset: After **3 seconds**, alert resets and servo returns to 0° if condition is SAFE.

---

## ⚙️ Installation

### 1. Install Arduino IDE
Download from [arduino.cc/en/software](https://www.arduino.cc/en/software)

### 2. Add ESP32 Board Support
In Arduino IDE → **File → Preferences → Additional Board Manager URLs**, add:
```
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```
Then go to **Tools → Board → Board Manager** → Search `esp32` → Install **esp32 by Espressif Systems**

### 3. Select Board
**Tools → Board → ESP32 Arduino → ESP32 Dev Module**

### 4. Install Libraries
Go to **Tools → Manage Libraries** and install:

| Library | Author | Purpose |
|---------|--------|---------|
| `ESP32Servo` | Kevin Harrington | Servo motor control |

> `WiFi` and `WebServer` are built-in with ESP32 board package — no separate install needed.

---

## 📦 Libraries Required

```cpp
#include <WiFi.h>        // Built-in — WiFi connection
#include <WebServer.h>   // Built-in — HTTP server on port 80
#include <ESP32Servo.h>  // Install via Library Manager
```

---

## 🚀 How to Run

### Step 1 — Configure WiFi in `.ino`
Open `Fire_and_Smoke_code.ino` and update:
```cpp
char ssid[] = "YourWiFiName";      // Your WiFi SSID
char pass[] = "YourWiFiPassword";  // Your WiFi Password
```

### Step 2 — Upload to ESP32
1. Connect ESP32 via USB
2. Select correct **Port** in Tools → Port
3. Click **Upload** (▶)
4. Open **Serial Monitor** at `115200` baud

### Step 3 — Note the IP Address
In Serial Monitor you will see:
```
=== Fire & Gas Safety System ===
[WiFi] Connecting to Oppo...
[WiFi] Connected!
[WiFi] ESP32 IP Address: http://192.168.1.47   ← COPY THIS
[Server] HTTP server started on port 80
[INFO] Waiting 3 minutes for MQ2 warm-up...
```

### Step 4 — Configure Dashboard
Open `pyros_dashboard.html` in any text editor and update line 1 of the script:
```javascript
const ESP32_IP = "192.168.1.47";   // ← Paste your ESP32 IP here
```

### Step 5 — Open Dashboard
Double-click `pyros_dashboard.html` — opens in your browser.
The dashboard will start fetching live data from ESP32 every 2 seconds.

> ✅ Both your PC and ESP32 **must be on the same WiFi network**

---

## 🌐 API Endpoint

The ESP32 exposes a REST API at port 80:

### `GET /data`
Returns current sensor readings as JSON:

```json
{
  "smokeAnalog":  423,
  "smokeDigital": 1,
  "flameAnalog":  3200,
  "flameDigital": 1,
  "status":       "SAFE",
  "alertType":    "NONE",
  "alertActive":  false,
  "smokeLimit":   500,
  "flameLimit":   500,
  "uptime":       145,
  "warmingUp":    false,
  "ip":           "192.168.1.47"
}
```

### `GET /`
Returns a simple HTML info page confirming ESP32 is running and showing its IP.

---

## 📊 Dashboard Features In Detail

| Feature | Description |
|---------|-------------|
| **Reactor Core** | Central animated ring arc fills proportionally to gas level. Rotates and pulses based on status. |
| **Thermometer Gauge** | Vertical tube with heat gradient fill. Threshold marker at smoke limit. Shows raw ADC, percentage, and peak. |
| **Flame Orb** | Three concentric rings pulse and glow when flame is detected. Static when clear. |
| **Servo Dial** | SVG half-circle gauge with needle. Shows 0° / 90° / 180° based on valve state. |
| **Waveform Chart** | Scrolling line chart (last 50 readings). Threshold dashed line overlay. Color changes with status. |
| **History Log** | Last 10 readings with timestamp, smoke ADC, flame yes/no, status pill, and note. Slide-in animation. |
| **Stats Row** | Total readings, peak ADC, alert events, dashboard uptime. Updates every second. |
| **Fire Background** | Full-screen cellular automaton fire simulation. Opacity increases on WARNING/CRITICAL. |
| **Bubble Canvas** | Lava-lamp style bubbles. Cyan = SAFE, Orange = WARNING, Red = CRITICAL. Speed increases with danger. |
| **Hazard Stripe** | Animated diagonal amber/red stripe appears at top on WARNING/CRITICAL. |
| **Screen Shake** | Entire UI shakes on CRITICAL. CSS keyframe animation. |
| **Danger Border** | Red glowing border pulses around viewport on CRITICAL. |
| **Ticker** | Scrolling status ticker showing system info and active monitoring message. |
| **Live Dot** | Green pulsing dot = connected. Red = offline. Updates on every successful fetch. |

---

## 📁 File Structure

```
fire-gas-monitor/
│
├── Fire_and_Smoke_code.ino     # ESP32 firmware (WebServer version)
├── pyros_dashboard.html        # Live web dashboard (open in browser)
└── README.md                   # This file
```

---

## 🔧 Configuration Reference

All configurable values are at the top of `Fire_and_Smoke_code.ino`:

```cpp
// WiFi
char ssid[] = "Oppo";
char pass[] = "55555555";

// Thresholds
int smokeLimit = 500;    // MQ2 analog value above which gas alert fires
int flameLimit = 500;    // Flame analog value below which fire alert fires

// Timing
unsigned long warmupTime  = 180000;  // MQ2 warm-up: 3 minutes (ms)
const int     alertDuration = 3000;  // Auto-reset alert after 3 seconds (ms)
```

And in `pyros_dashboard.html`:
```javascript
const ESP32_IP  = "192.168.1.100";  // Your ESP32's local IP
const POLL_MS   = 2000;             // Dashboard refresh rate (ms)
const GAS_THRESH = 500;             // Must match smokeLimit in .ino
```

---

## 🛠️ Troubleshooting

| Problem | Cause | Fix |
|---------|-------|-----|
| Dashboard shows OFFLINE | Wrong IP in HTML | Check Serial Monitor for correct IP, update `ESP32_IP` in HTML |
| ESP32 won't connect to WiFi | Wrong credentials or 5GHz network | ESP32 only supports **2.4GHz WiFi** — check SSID and password |
| Gas readings are 0 | MQ2 still warming up | Wait 3 minutes after boot — Serial Monitor shows `[WARMUP]` |
| Servo not moving | Power issue | Power servo from **5V external** — not ESP32 3.3V |
| Dashboard blank / no chart | Browser blocked fetch | Open browser console (F12) — check for CORS or network errors |
| Upload fails | Wrong board or port | Tools → Board → **ESP32 Dev Module** and correct COM port |
| Readings stuck after alert | Alert duration too long | Reduce `alertDuration` value in `.ino` |
| MQ2 always showing high | Calibration needed | Adjust `smokeLimit` — read baseline in clean air from Serial Monitor |

---

## 📐 Sensor Calibration Tips

### MQ2 Gas Sensor
1. Power on and let it warm up for **3 minutes minimum**
2. In clean air, note the `smokeAnalog` value from Serial Monitor (typically 100–400)
3. Set `smokeLimit` to about **2× the clean air reading**
4. Test with lighter gas (don't ignite!) to verify alert triggers

### Flame Sensor
1. Point away from any light source in normal conditions
2. Note `flameAnalog` value — should be **high** (2000–4000) when no flame
3. When flame is nearby, `flameAnalog` drops and `flameDigital` goes LOW
4. Adjust `flameLimit` if false triggers occur from ambient light

---

## 🧑‍💻 Tech Stack

| Layer | Technology |
|-------|-----------|
| Microcontroller | ESP32 (Xtensa LX6 dual-core 240MHz) |
| Firmware language | C++ (Arduino framework) |
| Communication | HTTP/1.1 over WiFi (port 80) |
| Dashboard | HTML5 + CSS3 + Vanilla JavaScript |
| Chart library | Chart.js v4.4.0 |
| Fonts | Black Ops One, Cousine, Barlow Condensed (Google Fonts) |
| Data format | JSON (REST API) |
| Hosting | ESP32 WebServer (no external server) |

---

## 🔒 Safety Notice

> ⚠️ This project is a **prototype for educational purposes**. It is **not a certified fire safety device**. Do not rely solely on this system for fire or gas safety in any building or critical environment. Always install certified smoke detectors and gas alarms as per local safety regulations.

---

## 📄 License

This project is licensed under the **MIT License** — feel free to use, modify, and distribute.

---

## 🙏 Acknowledgements

- [ESP32 Arduino Core](https://github.com/espressif/arduino-esp32) by Espressif
- [ESP32Servo Library](https://github.com/madhephaestus/ESP32Servo) by Kevin Harrington
- [Chart.js](https://www.chartjs.org/) for real-time waveform visualization
- MQ2 sensor datasheet — Zhengzhou Winsen Electronics

---

<div align="center">

**Made with 🔥 for IoT Safety**

*ESP32 + MQ2 + Flame Sensor + WebServer + PYROS Dashboard*

</div>
