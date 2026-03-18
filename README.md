# ♿ Arduino Smart Wheelchair

> **An open-source, low-cost assistive mobility system powered by Arduino — built for safety, accessibility, and real-time intelligence.**

![Arduino](https://img.shields.io/badge/Platform-Arduino-00979D?style=for-the-badge&logo=arduino&logoColor=white)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Active%20Development-orange?style=for-the-badge)
![PRs Welcome](https://img.shields.io/badge/PRs-Welcome-brightgreen?style=for-the-badge)

---

## 📌 Table of Contents

- [Overview](#-overview)
- [Features](#-features)
- [Components](#-components)
- [System Architecture](#-system-architecture)
- [Circuit Diagram](#-circuit-diagram)
- [Code Structure](#-code-structure)
- [Installation & Setup](#-installation--setup)
- [Usage](#-usage)
- [Future Roadmap](#-future-roadmap)
- [Research Extensions](#-research-extensions)
- [Contributing](#-contributing)
- [License](#-license)
- [Author](#-author)

---

## 🧭 Overview

The **Arduino Smart Wheelchair** is an embedded systems prototype designed to enhance the independence, safety, and quality of life for mobility-impaired individuals. By integrating ultrasonic sensors with real-time microcontroller logic, the system actively monitors the surrounding environment and responds to obstacles without requiring manual intervention.

This project is built for:
- 🔬 **Research & Prototyping** — Explore assistive technology at low cost
- 🎓 **Educational Use** — Learn embedded systems, sensor fusion, and robotics
- 🌐 **Community Contribution** — A foundation for open-source assistive hardware

The design is intentionally modular, making it straightforward to extend with AI/ML, wireless control, or IoT capabilities.

---

## ✨ Features

| Feature | Description |
|---|---|
| 🔍 **Obstacle Detection** | Real-time distance measurement using HC-SR04 ultrasonic sensors |
| 🚫 **Collision Avoidance** | Automatic motor stop or redirect when obstacle threshold is breached |
| ⚡ **Real-Time Response** | Low-latency processing on Arduino Uno/Nano |
| 🔊 **Alert System** | Buzzer and/or LED feedback for user and caregiver awareness |
| 🛠️ **Modular Architecture** | Easily expandable hardware and software layers |
| 🔋 **Battery Powered** | Fully portable and self-contained |
| 🤖 **AI/ML Ready** | Designed with future intelligent integration in mind |

---

## 🧰 Components

| Component | Quantity | Purpose |
|---|---|---|
| Arduino Uno / Nano | 1 | Main microcontroller |
| Ultrasonic Sensor (HC-SR04) | 1–3 | Distance/obstacle measurement |
| Motor Driver (L298N / L293D) | 1 | Motor speed and direction control |
| DC Motors | 2 | Wheel actuation |
| Wheels | 2 | Mobility |
| Battery Pack (7.4V–12V) | 1 | Power supply |
| Buzzer | Optional | Audible alerts |
| LEDs | Optional | Visual alerts |
| Jumper Wires | — | Connections |
| Wheelchair Chassis / Base | 1 | Structural frame |

---

## 🏗️ System Architecture

```
┌─────────────────────────────────────────────────────────┐
│                    INPUT LAYER                          │
│         HC-SR04 Ultrasonic Sensors (x1–3)              │
└─────────────────────────┬───────────────────────────────┘
                          │ Distance Data
                          ▼
┌─────────────────────────────────────────────────────────┐
│                  PROCESSING LAYER                       │
│              Arduino Uno / Nano                         │
│         - Reads sensor values                           │
│         - Applies threshold logic                       │
│         - Sends control signals                         │
└────────────┬──────────────────────┬─────────────────────┘
             │                      │
             ▼                      ▼
┌────────────────────┐   ┌─────────────────────────────┐
│   OUTPUT: MOTION   │   │      OUTPUT: ALERTS         │
│  L298N Motor Driver│   │   Buzzer / LED Indicators   │
│   → DC Motors      │   │   (Obstacle proximity)      │
│   → Wheels         │   │                             │
└────────────────────┘   └─────────────────────────────┘
```

### Working Principle

1. Ultrasonic sensors emit sound pulses and measure echo return time
2. Arduino calculates the distance to the nearest obstacle
3. If distance falls below a defined threshold (e.g., < 30 cm):
   - Motor driver halts or reroutes wheel movement
   - Buzzer/LED alert fires
4. If path is clear, normal movement resumes

---

## 🔌 Circuit Diagram

> 📌 *Place your circuit diagram image in `/docs/circuit_diagram.png` and it will render here.*

```
/docs/circuit_diagram.png
```

**HC-SR04 Pin Connections:**

| HC-SR04 Pin | Arduino Pin |
|---|---|
| VCC | 5V |
| GND | GND |
| TRIG | D9 |
| ECHO | D10 |

**L298N Motor Driver Connections:**

| L298N Pin | Arduino Pin |
|---|---|
| IN1 | D5 |
| IN2 | D6 |
| IN3 | D7 |
| IN4 | D8 |
| ENA / ENB | PWM Pins (D3, D11) |

> ⚠️ Always double-check your voltage levels before powering the circuit. The L298N requires a separate power supply for motors (7.4V–12V), not from Arduino's 5V pin.

---

## 💻 Code Structure

```
arduino-smart-wheelchair/
│
├── src/
│   └── main.ino               # Core Arduino sketch
│
├── docs/
│   ├── circuit_diagram.png    # Wiring diagram
│   └── architecture.md        # System design notes
│
├── assets/
│   ├── images/                # Photos / demo screenshots
│   └── videos/                # Demo recordings
│
├── README.md
└── LICENSE
```

---

## ⚙️ Installation & Setup

### Prerequisites

- [Arduino IDE](https://www.arduino.cc/en/software) (v1.8.x or v2.x)
- Git installed on your machine
- Arduino Uno or Nano board
- Hardware components listed above

---

### Step 1 — Clone the Repository

```bash
git clone https://github.com/your-username/arduino-smart-wheelchair.git
cd arduino-smart-wheelchair
```

### Step 2 — Open the Sketch

```bash
# Open in Arduino IDE
File → Open → src/main.ino
```

### Step 3 — Configure Board & Port

```
Tools → Board → Arduino Uno (or Nano)
Tools → Port → Select your COM port
```

### Step 4 — Assemble Hardware

- Connect all components as per the circuit diagram in `/docs/`
- Ensure motor driver has its own power supply
- Verify all GND connections are shared (common ground)

### Step 5 — Upload

Click the **Upload** button (→) or press `Ctrl + U`

---

## ▶️ Usage

1. **Power on** the system via battery pack
2. The Arduino begins reading sensor data immediately
3. Move the wheelchair forward — it will monitor surroundings continuously
4. On obstacle detection:
   - Motors auto-stop or reroute
   - Buzzer/LEDs activate (if connected)
5. Once the path clears, the system resumes normal operation

---

## 🗺️ Future Roadmap

- [ ] 🤖 **ML-based Obstacle Prediction** — Classify terrain and predict obstacles proactively
- [ ] 📱 **Bluetooth / WiFi App Control** — Remote override and monitoring via mobile
- [ ] 🧭 **Autonomous Path Planning** — Integrate algorithms like A* or Dijkstra
- [ ] 🛰️ **GPS Integration** — Location tracking and geofencing for caregivers
- [ ] 🗣️ **Voice Command System** — Hands-free directional control
- [ ] 📡 **IoT Dashboard** — Real-time sensor data logging and remote monitoring
- [ ] ⏳ **RUL Prediction** — Machine learning to predict sensor and motor degradation

---

## 🔬 Research Extensions

This project serves as a foundation for advanced research in:

- **Remaining Useful Life (RUL) Prediction** — ML models for predictive maintenance of sensors/motors
- **Environmental Interference Testing** — Effect of dust, temperature, and material on sensor accuracy
- **Sensor Fusion** — Combining ultrasonic, IR, and camera data for robust perception
- **Failure Mode Analysis** — Detecting anomalies using classification models
- **Assistive Healthcare Devices** — Broader application in smart mobility aids

---

## 🤝 Contributing

Contributions are warmly welcomed! Whether it's fixing bugs, improving documentation, or adding new features:

1. **Fork** this repository
2. **Create** a feature branch: `git checkout -b feature/your-feature-name`
3. **Commit** your changes: `git commit -m "Add: your feature description"`
4. **Push** to your branch: `git push origin feature/your-feature-name`
5. **Open** a Pull Request — describe what you changed and why

Please follow consistent code formatting and comment your Arduino code clearly.

---

## 📄 License

This project is licensed under the **MIT License** — free to use, modify, and distribute with attribution.

See [`LICENSE`](./LICENSE) for full terms.

---

## 👤 Author

**Reshav Koley**

> Student · Embedded Systems Developer · Robotics & AI Enthusiast

- 💡 Passionate about building accessible, real-world technology
- 🔧 Focused on Embedded Systems, IoT, and Machine Learning
- 📬 Open to collaboration, feedback, and research partnerships

---

## ⭐ Show Your Support

If this project helped you or inspired your work, please consider giving it a **⭐ star** on GitHub — it helps others discover it and motivates continued development.

---

*Built with purpose. Designed for accessibility. Open for everyone.*