# ♿ Arduino Smart Wheelchair

> **A multi-modal, intelligent assistive mobility system — controlled by keypad, joystick, or hand gestures, with real-time obstacle avoidance and GPS/GSM-powered emergency response.**

![Arduino](https://img.shields.io/badge/Platform-Arduino-00979D?style=for-the-badge&logo=arduino&logoColor=white)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Active%20Development-orange?style=for-the-badge)
![PRs Welcome](https://img.shields.io/badge/PRs-Welcome-brightgreen?style=for-the-badge)
![Control Modes](https://img.shields.io/badge/Control%20Modes-3-blueviolet?style=for-the-badge)
![Emergency](https://img.shields.io/badge/Emergency-GPS%20%2B%20GSM-red?style=for-the-badge)

---

## 🧭 Overview

The **Arduino Smart Wheelchair** is a comprehensive embedded systems platform that transforms a standard wheelchair into a smart, responsive, and life-saving mobility device. It supports **three independent control modes** — keypad, joystick, and gesture — letting users choose the input method that best suits their physical capability.

At its core, the system features:
- Real-time **obstacle detection and avoidance**
- A dedicated **GPS + GSM emergency response module**
- **Multi-modal control** switchable on the fly
- Fully **modular and open-source** hardware/software design

This project is built for:
- 🔬 **Research & Prototyping** — Multi-input assistive mobility systems
- 🎓 **Educational Use** — Sensor fusion, GSM communication, embedded control
- 🏥 **Healthcare Innovation** — Real-world assistive technology with emergency capabilities
- 🌐 **Community Contribution** — An open platform for accessible mobility

---

## 🌟 System Highlights

```
  ┌──────────────────────────────────────────────────────────────────────┐
  │                     3 CONTROL MODES                                  │
  │   ⌨️ Keypad    🕹️ Joystick    🤚 Gesture (MPU6050 / Flex Sensor)    │
  ├──────────────────────────────────────────────────────────────────────┤
  │                     SAFETY SYSTEMS                                   │
  │   🔍 Obstacle Detection    🚫 Collision Avoidance    🔊 Alerts       │
  ├──────────────────────────────────────────────────────────────────────┤
  │                     EMERGENCY RESPONSE                               │
  │   🛰️ GPS Location    📡 GSM SMS Alert    📞 Emergency Call           │
  └──────────────────────────────────────────────────────────────────────┘
```

---

## ✨ Features

| Category | Feature | Description |
|---|---|---|
| 🎮 **Control** | Keypad Input | 4×4 matrix keypad for directional and mode commands |
| 🎮 **Control** | Joystick Navigation | Analog joystick for smooth, proportional movement control |
| 🎮 **Control** | Gesture Control | MPU6050 IMU / flex sensors detect hand tilt and motion |
| 🔍 **Safety** | Obstacle Detection | HC-SR04 ultrasonic sensors for front and side proximity sensing |
| 🚫 **Safety** | Collision Avoidance | Auto-halt and reroute logic triggered at configurable threshold |
| 🔊 **Safety** | Alert System | Buzzer + LED feedback on obstacle detection or emergency events |
| 🛰️ **Emergency** | GPS Tracking | NEO-6M module provides real-time location coordinates |
| 📡 **Emergency** | GSM SMS Alert | SIM800L sends location + SOS message to caregiver on demand |
| 📞 **Emergency** | Emergency Call | Automated call to registered emergency contact |
| ⚡ **Core** | Real-Time Processing | Low-latency Arduino control loop across all subsystems |
| 🛠️ **Design** | Modular Architecture | Each subsystem is independently upgradeable |
| 🔋 **Power** | Battery-Powered | Portable, self-contained operation |

---

## 🧰 Components

### Core Electronics

| Component | Quantity | Purpose |
|---|---|---|
| Arduino Mega 2560 | 1 | Main microcontroller (recommended for pin count) |
| HC-SR04 Ultrasonic Sensor |1 | Front |
| L298N Motor Driver | 1 | Dual DC motor speed and direction control |
| DC Motors (geared) | 2 | Wheel actuation |
| Wheels + Chassis | 1 set | Structural mobility base |
| Battery Pack (11.1V 3S LiPo) | 1 | Main power supply |
| 5V Voltage Regulator (LM7805) | 1 | Clean 5V for Arduino logic |
| Buzzer | 1 | Audible alerts |
| LEDs (Red / Green) | 2–4 | Visual status indicators |

### Control Inputs

| Component | Quantity | Purpose |
|---|---|---|
| 4×4 Matrix Keypad | 1 | Directional + mode switching commands |
| Analog Joystick KY-023 | 1 | Smooth proportional movement |
| MPU6050 IMU Module | 1 | Gesture / tilt-based control |
| Flex Sensor | Optional | Finger-bend gesture alternative |
| Mode Toggle Switch | 1 | Hardware control mode selector |

### Emergency & Communication

| Component | Quantity | Purpose |
|---|---|---|
| NEO-6M GPS Module | 1 | Real-time location coordinates |
| SIM800L GSM Module | 1 | SMS + voice call emergency alerts |
| 2G SIM Card | 1 | GSM network access |
| 3.7V LiPo 2000mAh | 1 | Dedicated power for GSM module |
| SOS Push Button | 1 | Dedicated hardware emergency trigger |

### Miscellaneous

| Component | Quantity | Purpose |
|---|---|---|
| Jumper Wires | — | Connections |
| Breadboard / Perfboard | 1 | Prototyping / final assembly |
| 10kΩ Resistors | Several | Pull-downs, voltage dividers |

---

## 🏗️ System Architecture

```
╔══════════════════════════════════════════════════════════════════════╗
║                        INPUT / CONTROL LAYER                        ║
╠══════════════════╦══════════════════╦══════════════════╦════════════╣
║  ⌨️  KEYPAD      ║  🕹️  JOYSTICK    ║  🤚 GESTURE IMU  ║ 🚨 SOS    ║
║  4×4 Matrix      ║  KY-023 Analog   ║  MPU6050 / Flex  ║ Button    ║
╚══════════════════╩══════════════════╩══════════════════╩════════════╣
                                    │
                                    ▼
╔══════════════════════════════════════════════════════════════════════╗
║                      SAFETY / SENSOR LAYER                          ║
║          HC-SR04 Ultrasonic Sensors (Front + Left + Right)          ║
║           Continuous distance measurement & collision logic          ║
╚══════════════════════════════════════════════════════════════════════╣
                                    │
                                    ▼
╔══════════════════════════════════════════════════════════════════════╗
║                       PROCESSING LAYER                              ║
║                     Arduino Mega 2560                               ║
║  ┌──────────────────────────────────────────────────────────────┐   ║
║  │  • Parse active control mode (Keypad / Joystick / Gesture)   │   ║
║  │  • Poll ultrasonic sensors every loop cycle                  │   ║
║  │  • Apply obstacle threshold & avoidance logic                │   ║
║  │  • Handle SOS → GPS read → GSM SMS → Call                   │   ║
║  │  • Output PWM signals to motor driver                        │   ║
║  └──────────────────────────────────────────────────────────────┘   ║
╚══════════════════════════════════════════════════════════════════════╣
               │                                    │
               ▼                                    ▼
╔══════════════════════════╗         ╔══════════════════════════════╗
║     MOTION OUTPUT        ║         ║     EMERGENCY OUTPUT         ║
║  L298N Motor Driver      ║         ║  NEO-6M  → GPS coordinates   ║
║  → Left DC Motor (PWM)   ║         ║  SIM800L → SMS to caregiver  ║
║  → Right DC Motor (PWM)  ║         ║  SIM800L → Voice call        ║
║  → Differential steering ║         ║  Buzzer  → Local SOS alert   ║
╚══════════════════════════╝         ╚══════════════════════════════╝
```

**Control Priority (highest → lowest):**

```
  SOS Trigger       →  Always overrides all motion immediately
  Obstacle Detected →  Overrides any active motion command
  Active Mode Input →  Keypad / Joystick / Gesture
  Idle / Stop       →  Default state
```

## 📚 Required Libraries

Install via **Arduino IDE → Sketch → Include Library → Manage Libraries**:

| Library | Purpose | Author |
|---|---|---|
| `Keypad` | 4×4 matrix keypad input | Mark Stanley |
| `MPU6050` | IMU gesture / tilt reading | Electronic Cats |
| `Wire` | I²C communication | Built-in |
| `TinyGPSPlus` | GPS NMEA sentence parsing | Mikal Hart |
| `NewPing` | Ultrasonic HC-SR04 sensing | Tim Eckel |
| `SoftwareSerial` | Extra UART for GSM/GPS | Built-in |

---

## ⚙️ Installation & Setup

### Prerequisites

- [Arduino IDE](https://www.arduino.cc/en/software) v1.8.x or v2.x
- Arduino Mega 2560 board
- All hardware components listed above
- Active 2G SIM card inserted in SIM800L
- Git installed on your machine

---

### Step 1 — Clone the Repository

```bash
git clone https://github.com/your-username/arduino-smart-wheelchair.git
cd arduino-smart-wheelchair
```

### Step 2 — Configure `config.h`

```cpp
// Emergency contact
#define EMERGENCY_NUMBER    "+91XXXXXXXXXX"
#define OWNER_NAME          "Reshav Koley"

// Obstacle avoidance threshold (cm)
#define OBSTACLE_THRESHOLD  30

// Joystick dead zone (0–1023 scale)
#define JOYSTICK_DEADZONE   50
#define JOYSTICK_CENTER     512

// Gesture tilt sensitivity (degrees)
#define GESTURE_TILT_ANGLE  20

// Default startup mode: 0=Keypad | 1=Joystick | 2=Gesture
#define DEFAULT_MODE        0
```

### Step 3 — Install Libraries

```
Arduino IDE → Sketch → Include Library → Manage Libraries
Search and install: Keypad, MPU6050, TinyGPSPlus, NewPing
```

### Step 4 — Assemble Hardware

- Wire all components per the Pin Reference table above
- Power SIM800L from a dedicated LiPo (3.7–4.2V, 2A rated)
- Confirm all modules share common GND
- Position GPS module with clear line of sight to open sky

### Step 5 — Select Board & Upload

```
Tools → Board    → Arduino Mega or Mega 2560
Tools → Port     → Select your COM port
Sketch → Upload  (Ctrl+U)
```

---

## ▶️ Usage

### Powering On
1. Connect the main battery pack
2. System initializes all modules (~3 seconds)
3. Green LED blinks 3× → system ready
4. Default control mode activates (Keypad unless changed in `config.h`)

### Switching Control Modes
- Press `#` on keypad → cycles: **Keypad → Joystick → Gesture → Keypad**
- Or flip the physical mode toggle switch

### Driving

| Mode | How to Control |
|---|---|
| **Keypad** | `2` forward, `8` backward, `4` left, `6` right, `5` stop |
| **Joystick** | Push in direction of travel; release to stop |
| **Gesture** | Tilt hand/wrist in desired direction; hold flat to stop |

### Triggering Emergency SOS

| Method | Action |
|---|---|
| Keypad | Press `*` |
| Joystick | Long-press joystick button (> 2 sec) |
| Gesture | Sharp wrist shake or jerk |
| Hardware Button | Press red SOS button (D35) |

After SOS: GPS locks → SMS dispatched → Call initiated → Buzzer activates. Red LED stays solid. Press `0` on keypad to cancel once safe.

### Obstacle Avoidance
Runs **automatically in the background** in all modes. When an obstacle is within the threshold distance:
- Motors halt immediately
- Buzzer beeps twice, red LED flashes
- Movement resumes once path is clear

---

## 🗺️ Future Roadmap

- [x] ⌨️ Keypad directional control
- [x] 🕹️ Joystick proportional movement
- [x] 🤚 Gesture / IMU-based control
- [x] 🛰️ GPS real-time location
- [x] 📡 GSM SMS + voice call emergency
- [x] 🔍 Ultrasonic obstacle avoidance
- [ ] 📱 **Bluetooth / WiFi Mobile App** — Live status dashboard + remote override
- [ ] 🗣️ **Voice Command Control** — Directional input via microphone module
- [ ] 🤖 **ML Obstacle Classification** — Predict and categorize terrain types
- [ ] 🧭 **Autonomous Path Planning** — A* / Dijkstra navigation integration
- [ ] 📊 **IoT Health Dashboard** — Battery level, GPS track log, usage analytics
- [ ] ⏳ **RUL Prediction** — ML model for sensor/motor degradation forecasting
- [ ] 🔋 **Solar Charging Panel** — Supplemental renewable energy
- [ ] 🧠 **BCI Integration** — EEG-based control (experimental research track)

---

## 🔬 Research Extensions

This project provides a strong platform for:

- **Remaining Useful Life (RUL) Prediction** — Train ML models on motor current and sensor readings to forecast component failure
- **Multi-Modal Input Comparison** — Study user fatigue, accuracy, and preference across keypad, joystick, and gesture modes
- **Sensor Fusion** — Combine ultrasonic, IR, and camera data for richer 3D environment mapping
- **GSM Emergency Latency Testing** — Measure SMS delivery time and call reliability across network conditions
- **Environmental Interference Studies** — Assess how temperature, humidity, and surface material affect HC-SR04 accuracy
- **Gesture Recognition Benchmarking** — Compare IMU vs. flex sensor vs. EMG for gesture reliability and user comfort
- **Assistive Healthcare Robotics** — Adapt platform for hospital bed actuation, patient transport, or rehabilitation support

---

## 🤝 Contributing

All contributions are welcome — hardware improvements, new control modes, documentation, or companion apps!

1. **Fork** this repository
2. **Create** a feature branch: `git checkout -b feature/your-feature-name`
3. **Commit** your changes: `git commit -m "Add: your feature description"`
4. **Push** to your branch: `git push origin feature/your-feature-name`
5. **Open** a Pull Request — describe what you changed and why

**Good areas to contribute:**
- Improved gesture recognition or ML-based classification
- Additional GSM module support (SIM900, A6, EC21)
- Mobile app companion (Flutter / React Native)
- Schematic improvements or KiCad PCB design
- Translations and documentation in other languages

Please comment your Arduino code clearly and maintain the existing file structure.

---

## 📄 License

This project is licensed under the **MIT License** — free to use, modify, and distribute with attribution.

See [`LICENSE`](./LICENSE) for full terms.

---

## 👤 Author

**Reshav Koley**

> Student · Embedded Systems Developer · Robotics & AI Enthusiast

- 💡 Passionate about building accessible, life-improving technology
- 🔧 Focused on Embedded Systems, IoT, Sensor Fusion, and Machine Learning
- 📬 Open to collaboration, research partnerships, and feedback

---

## ⭐ Show Your Support

If this project helped, inspired, or informed your work, please consider giving it a **⭐ star** on GitHub — it helps others discover it and motivates continued development.

---

*Built with purpose. Designed for independence. Open for everyone.*
