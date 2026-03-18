/**
 * ============================================================
 *  Smart Wheelchair Controller
 * ============================================================
 *  Platform  : Arduino Mega 2560
 *  Author    : Reshav Koley
 *  Version   : 1.0
 *
 *  Description:
 *    This program controls a smart wheelchair with the following
 *    features:
 *      - Dual control modes: Joystick or Gesture (MPU6050)
 *      - Obstacle detection via HC-SR04 ultrasonic sensor
 *      - Buzzer alert when an obstacle is within 30 cm
 *      - Emergency SMS + GPS location via SIM800L GSM module
 *      - Emergency voice call via SIM800L
 *      - Mode & action selection via a 4x4 matrix keypad
 *
 *  Keypad Controls:
 *    '1' → Send emergency SMS with live GPS location
 *    '2' → Make emergency phone call
 *    '3' → Stop / Kill switch
 *    '5' → Switch to Gesture Control mode (MPU6050)
 *    '6' → Switch to Joystick Control mode
 *
 *  Libraries Required (install via Arduino Library Manager):
 *    - Keypad            by Mark Stanley
 *    - I2Cdev            by Jeff Rowberg
 *    - MPU6050           by Jeff Rowberg
 *    - TinyGPS++         by Mikal Hart
 *    - SoftwareSerial    (built-in)
 *    - Wire              (built-in)
 *
 *  Wiring Summary:
 *    Ultrasonic (HC-SR04) : Trig → A14, Echo → A15
 *    Buzzer               : A7
 *    Joystick             : X → A1, Y → A2
 *    Keypad Rows          : Pins 9, 10, 11, 12
 *    Keypad Cols          : Pins 31, 32, 33, 34
 *    Motor Driver (L298N) : See motor pin definitions below
 *    MPU6050 (I2C)        : SDA → Pin 20, SCL → Pin 21
 *    GPS Module           : Serial1 (Pins 18 TX, 19 RX) at 9600 baud
 *    SIM800L GSM          : RX → Pin 2, TX → Pin 3 (SoftwareSerial)
 * ============================================================
 */

// ─── Library Includes ────────────────────────────────────────
#include <Keypad.h>          // 4x4 matrix keypad input
#include "Wire.h"            // I2C communication (for MPU6050)
#include "I2Cdev.h"          // I2C device helper (MPU6050 dependency)
#include "MPU6050.h"         // Accelerometer/Gyroscope for gesture control
#include <SoftwareSerial.h>  // Software UART for SIM800L GSM module
#include <TinyGPS++.h>       // GPS NMEA sentence parsing

// ─── GPS Setup ───────────────────────────────────────────────
TinyGPSPlus gps;             // GPS parser object (reads from Serial1)

String lati  = "";           // Stores latitude string for SMS
String longi = "";           // Stores longitude string for SMS

// ─── Ultrasonic Sensor (HC-SR04) Pins ────────────────────────
#define echoPin A15   // Echo pin: receives the reflected pulse
#define trigPin A14   // Trigger pin: sends the ultrasonic pulse

// ─── Buzzer Pin ──────────────────────────────────────────────
#define buzzer A7     // Active buzzer; alerts when obstacle < 30 cm

// ─── Ultrasonic Measurement Variables ────────────────────────
long duration;    // Time (µs) for echo pulse to return
int  distance;    // Calculated distance in centimetres

// ─── Accelerometer / Gyroscope (MPU6050) ─────────────────────
MPU6050 mpu;              // MPU6050 object (I2C address 0x68 by default)
int16_t ax, ay, az;       // Raw accelerometer values (X, Y, Z axes)
int16_t gx, gy, gz;       // Raw gyroscope values (not used in logic, but read)

/**
 * MyData struct
 * Holds MPU6050 axis values mapped to the 0–255 byte range.
 * This makes threshold comparisons simpler and more readable.
 */
struct MyData {
  byte X;   // Mapped accelerometer X axis (0–255)
  byte Y;   // Mapped accelerometer Y axis (0–255)
  byte Z;   // Mapped accelerometer Z axis (0–255)
};
MyData data;

// ─── Keypad Configuration ────────────────────────────────────
const byte ROWS = 4;  // Number of keypad rows
const byte COLS = 4;  // Number of keypad columns

// Key layout matching the physical keypad face
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

// Arduino Mega pins connected to keypad column wires
byte colPins[COLS] = { 31, 32, 33, 34 };
// Arduino Mega pins connected to keypad row wires
byte rowPins[ROWS] = { 9, 10, 11, 12 };

// Instantiate the Keypad object with the above configuration
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ─── Motor Driver Pins (L298N or similar dual H-bridge) ──────
// Each motor needs two direction pins (IN1/IN2).
// HIGH/LOW combinations determine forward or backward rotation.
//
//  Motor 1 & 2 → Right side wheels
//  Motor 3 & 4 → Left side wheels
int motor1pin1 = 36;  // Motor 1 direction pin A
int motor1pin2 = 38;  // Motor 1 direction pin B

int motor2pin1 = 40;  // Motor 2 direction pin A
int motor2pin2 = 42;  // Motor 2 direction pin B

int motor3pin1 = 52;  // Motor 3 direction pin A
int motor3pin2 = 50;  // Motor 3 direction pin B

int motor4pin1 = 48;  // Motor 4 direction pin A
int motor4pin2 = 46;  // Motor 4 direction pin B

// ─── Motor Speed Control (PWM Enable Pins) ───────────────────
// These pins control motor speed via PWM (analogWrite).
// EN1 controls Motor 1 & 2 speed; EN2 & EN3 control Motor 3 & 4.
int EN1 = 7;   // PWM enable for motors 1 & 2 (right side)
int EN2 = 6;   // PWM enable for motors 3 & 4 (left side)
int EN3 = 8;   // Additional PWM channel (adjust as needed)

// ─── Joystick Analog Pins & Neutral Values ───────────────────
// Neutral (resting) ADC values; calibrate these for your joystick.
int xVal = 340;    // Current X-axis ADC reading
int yVal = 338;    // Current Y-axis ADC reading
int xPin = A1;     // Joystick X-axis analog input pin
int yPin = A2;     // Joystick Y-axis analog input pin

// ─── Control Mode ────────────────────────────────────────────
// 0 = no mode (stopped), 1 = joystick, 2 = gesture, 3 = stop
int mode = 0;

// ─── GSM Module (SIM800L) via SoftwareSerial ─────────────────
#define rxPin 2   // Arduino RX ← SIM800L TX
#define txPin 3   // Arduino TX → SIM800L RX
SoftwareSerial sim800L(rxPin, txPin);

// ─────────────────────────────────────────────────────────────
//  SETUP
// ─────────────────────────────────────────────────────────────
void setup() {
  // Start hardware serial for GPS module (connected to Serial1 port)
  Serial1.begin(9600);

  // Start Serial monitor for debugging
  Serial.begin(9600);
  Serial.println("Mega up");

  // ── Pin Modes ────────────────────────────────────────────
  pinMode(buzzer, OUTPUT);

  // Motor direction pins
  pinMode(motor1pin1, OUTPUT);  pinMode(motor1pin2, OUTPUT);
  pinMode(motor2pin1, OUTPUT);  pinMode(motor2pin2, OUTPUT);
  pinMode(motor3pin1, OUTPUT);  pinMode(motor3pin2, OUTPUT);
  pinMode(motor4pin1, OUTPUT);  pinMode(motor4pin2, OUTPUT);

  // Motor speed (PWM) pins
  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT);
  pinMode(EN3, OUTPUT);

  // Ultrasonic sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Built-in LED (can be used for status indication)
  pinMode(LED_BUILTIN, OUTPUT);

  // ── MPU6050 Initialisation ───────────────────────────────
  Wire.begin();
  mpu.initialize();

  // ── SIM800L GSM Initialisation ──────────────────────────
  sim800L.begin(9600);
  Serial.println("Initializing GSM module...");

  // Verify communication with the modem
  sim800L.println("AT");
  waitForResponse();

  // Enable command echo (useful for debugging AT commands)
  sim800L.println("ATE1");
  waitForResponse();

  // Set SMS to text mode (as opposed to PDU mode)
  sim800L.println("AT+CMGF=1");
  waitForResponse();

  // Configure new SMS notification: display directly to serial output
  sim800L.println("AT+CNMI=1,2,0,0,0");
  waitForResponse();

  Serial.println("Setup complete. Press '6' for Joystick, '5' for Gesture.");
}

// ─────────────────────────────────────────────────────────────
//  MAIN LOOP
// ─────────────────────────────────────────────────────────────
void loop() {
  // ── Set Motor Speeds via PWM ─────────────────────────────
  // Values 0–255; adjust to suit your motor driver and battery voltage.
  analogWrite(EN1, 100);   // Right-side motors at ~39% speed
  analogWrite(EN2, 200);   // Left-side motors at ~78% speed
  analogWrite(EN3, 200);   // Additional motor channel

  // ── Ultrasonic Distance Measurement ─────────────────────
  // Send a 10 µs HIGH pulse on trigPin to trigger the sensor.
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);          // Ensure a clean LOW before trigger
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);         // 10 µs pulse width (required by HC-SR04)
  digitalWrite(trigPin, LOW);

  // Measure time (µs) for the echo pulse to return.
  // pulseIn() blocks until the echo pin goes HIGH then LOW again.
  duration = pulseIn(echoPin, HIGH);

  // Convert time to distance: distance = (time × speed_of_sound) / 2
  // Speed of sound ≈ 0.0344 cm/µs; divide by 2 for round-trip.
  distance = duration * 0.0344 / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  delay(100);  // Short pause between measurements

  // ── Keypad Input: Switch Modes / Trigger Actions ─────────
  readKeypad();

  // ── Drive Motors According to Active Control Mode ────────
  if (mode == 1) {
    joystickControl();     // Analog joystick drives the chair
  } else if (mode == 2) {
    gestureControl();      // MPU6050 tilt controls the chair
  } else {
    stop();                // mode 0, 3, or unknown → stop motors
  }

  // ── Obstacle Alert ───────────────────────────────────────
  // If an obstacle is detected within 30 cm, sound the buzzer.
  if (distance < 30) {
    digitalWrite(buzzer, HIGH);
    delay(1000);                 // Buzzer ON for 1 second
    digitalWrite(buzzer, LOW);
    delay(500);                  // Brief pause before next cycle
  }
}

// ─────────────────────────────────────────────────────────────
//  JOYSTICK CONTROL
//  Reads the analog joystick and moves the chair accordingly.
//  Movement is blocked if an obstacle is closer than 30 cm.
// ─────────────────────────────────────────────────────────────
void joystickControl() {
  xVal = analogRead(xPin);   // Read X axis (forward / backward)
  yVal = analogRead(yPin);   // Read Y axis (left / right)

  Serial.print("X: "); Serial.print(xVal);
  Serial.print("  Y: "); Serial.println(yVal);

  // Thresholds account for joystick mechanical deadband.
  // Adjust the values (345, 335, 343, 333) to match your joystick's
  // actual neutral ADC reading ± a comfortable deadband margin.

  if (xVal >= 345 && distance >= 30)  forward();
  if (xVal <= 335 && distance >= 30)  backward();
  if (yVal >= 343 && distance >= 30)  right();
  if (yVal <= 333 && distance >= 30)  left();

  // Stop if joystick is in the neutral zone OR obstacle is too close
  if ((xVal < 345 && xVal > 335 && yVal < 343 && yVal > 333) || distance < 30) {
    stop();
  }
}

// ─────────────────────────────────────────────────────────────
//  GESTURE CONTROL
//  Uses MPU6050 accelerometer data to steer the wheelchair by
//  physically tilting the controller module.
//
//  Tilt forward  → Chair moves forward
//  Tilt backward → Chair moves backward
//  Tilt right    → Chair turns right
//  Tilt left     → Chair turns left
//  Level / flat  → Chair stops
//
//  The raw ±17000 ADC range is mapped to 0–255 for easy threshold
//  comparisons. Adjust thresholds if your MPU6050 is oriented
//  differently or mounted at a different angle.
// ─────────────────────────────────────────────────────────────
void gestureControl() {
  // Read all 6 axes from the MPU6050 in one call
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // Map raw accelerometer values (-17000 to +17000) → (0 to 255)
  data.X = map(ax, -17000, 17000, 0, 255);
  data.Y = map(ay, -17000, 17000, 0, 255);
  data.Z = map(az, -17000, 17000, 0, 255);  // Z not used in control logic

  Serial.print("Axis X="); Serial.print(data.X);
  Serial.print("  Y=");    Serial.print(data.Y);
  Serial.print("  Z=");    Serial.println(data.Z);

  // ── Movement Decisions ───────────────────────────────────
  // Forward: Y tilted high (≥180) while X stays centred (≤160)
  if (data.Y >= 180 && data.X <= 160 && distance >= 30)  forward();

  // Backward: Y tilted low (≤100) while X stays centred (≤160)
  if (data.Y <= 100 && data.X <= 160 && distance >= 30)  backward();

  // Right: X tilted high (≥190) while Y stays centred (≤170)
  if (data.X >= 190 && data.Y <= 170 && distance >= 30)  right();

  // Left: X tilted low (≤100) while Y stays centred (≤170)
  if (data.X <= 100 && data.Y <= 170 && distance >= 30)  left();

  // Stop: device is roughly level, or obstacle is too close
  if ((data.X < 190 && data.X > 100 && data.Y > 100 && data.Y < 180) || distance < 30) {
    stop();
  }
}

// ─────────────────────────────────────────────────────────────
//  KEYPAD HANDLER
//  Polls the 4x4 keypad and acts on recognised key presses:
//    '1' → Send emergency SMS
//    '2' → Make emergency phone call
//    '3' → Force stop (mode 3)
//    '5' → Gesture control mode
//    '6' → Joystick control mode
// ─────────────────────────────────────────────────────────────
void readKeypad() {
  char key = keypad.getKey();   // Returns '\0' if no key is pressed

  if (key) {
    Serial.print("Key Pressed: "); Serial.println(key);

    switch (key) {
      case '6':
        mode = 1;
        Serial.println("MODE: JOYSTICK CONTROL");
        delay(200);   // Debounce: ignore bounce within 200 ms
        break;

      case '5':
        mode = 2;
        Serial.println("MODE: GESTURE CONTROL");
        delay(200);
        break;

      case '3':
        mode = 3;
        Serial.println("MODE: STOP (Kill Switch)");
        break;

      case '2':
        make_call();   // Dial the emergency number
        break;

      case '1':
        send_sms();    // Send GPS-tagged emergency SMS
        break;

      // All other keys are intentionally ignored
    }
  }
}

// ─────────────────────────────────────────────────────────────
//  SEND EMERGENCY SMS
//  Reads the latest GPS fix from Serial1 (hardware UART connected
//  to the GPS module), then sends an SMS with a Google Maps link
//  to the hard-coded emergency contact number.
//
//  NOTE: Replace "+919932476089" with the actual contact number.
//        The number must include the country code (e.g. +91 for India).
// ─────────────────────────────────────────────────────────────
void send_sms() {
  // Try to read a fresh GPS sentence before sending
  if (Serial1.available()) {
    gps.encode(Serial1.read());           // Feed raw NMEA bytes to TinyGPS++
    lati  = String(gps.location.lat(), 6); // 6 decimal places ≈ ~11 cm accuracy
    longi = String(gps.location.lng(), 6);
  }

  // Open SMS to the emergency number
  sim800L.print("AT+CMGS=\"+919932476089\"\r");
  waitForResponse();

  Serial.println("Sending SMS...");

  // Compose and send the SMS body with an embedded Google Maps link
  sim800L.print(
    "Emergency Alert! The user of the smart wheelchair needs assistance. "
    "Location: https://maps.google.com/?q=" + lati + "," + longi +
    ". Please respond immediately."
  );

  // Ctrl+Z (0x1A) signals the end of the SMS to the modem
  sim800L.write(0x1A);
  waitForResponse();

  Serial.println("SMS sent.");
}

// ─────────────────────────────────────────────────────────────
//  MAKE EMERGENCY CALL
//  Dials the emergency contact using the ATD (Dial) command.
//  The trailing semicolon (;) keeps the call in voice mode.
//
//  NOTE: Replace "+919932476089" with the actual contact number.
// ─────────────────────────────────────────────────────────────
void make_call() {
  sim800L.println("ATD+919932476089;");
  waitForResponse();
  Serial.println("Calling emergency contact...");
}

// ─────────────────────────────────────────────────────────────
//  WAIT FOR GSM RESPONSE
//  Pauses for 1 second to allow the SIM800L to process the last
//  AT command, then reads and prints any response text to Serial.
//  Called after every AT command to ensure proper sequencing.
// ─────────────────────────────────────────────────────────────
void waitForResponse() {
  delay(1000);                        // Wait for the modem to respond
  while (sim800L.available()) {
    Serial.println(sim800L.readString());   // Print modem response for debugging
  }
  sim800L.read();                     // Flush any remaining byte in buffer
}

// ─────────────────────────────────────────────────────────────
//  MOTOR CONTROL FUNCTIONS
//  Each function sets the H-bridge direction pins to produce
//  the desired wheel movement.
//
//  Motor wiring convention used here:
//    Motors 1 & 2 → Right-side wheels
//    Motors 3 & 4 → Left-side wheels
//
//  forward()  : All motors spin forward
//  backward() : All motors spin backward
//  stop()     : All motors off (coast stop)
//  left()     : Only right-side motors run → turns left
//  right()    : Only left-side motors run  → turns right
// ─────────────────────────────────────────────────────────────

/** Move all four wheels forward */
void forward() {
  digitalWrite(motor1pin1, HIGH);  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, HIGH);  digitalWrite(motor2pin2, LOW);
  digitalWrite(motor3pin1, HIGH);  digitalWrite(motor3pin2, LOW);
  digitalWrite(motor4pin1, HIGH);  digitalWrite(motor4pin2, LOW);
}

/** Move all four wheels backward */
void backward() {
  digitalWrite(motor1pin1, LOW);   digitalWrite(motor1pin2, HIGH);
  digitalWrite(motor2pin1, LOW);   digitalWrite(motor2pin2, HIGH);
  digitalWrite(motor3pin1, LOW);   digitalWrite(motor3pin2, HIGH);
  digitalWrite(motor4pin1, LOW);   digitalWrite(motor4pin2, HIGH);
}

/** Stop all motors (coast — both pins LOW means no current through motor) */
void stop() {
  digitalWrite(motor1pin1, LOW);  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, LOW);  digitalWrite(motor2pin2, LOW);
  digitalWrite(motor3pin1, LOW);  digitalWrite(motor3pin2, LOW);
  digitalWrite(motor4pin1, LOW);  digitalWrite(motor4pin2, LOW);
}

/**
 * Turn left
 * Left-side wheels stop; right-side wheels drive forward.
 * The wheelchair pivots to the left.
 */
void left() {
  // Right-side motors: stop
  digitalWrite(motor1pin1, LOW);  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, LOW);  digitalWrite(motor2pin2, LOW);
  // Left-side motors: forward
  digitalWrite(motor3pin1, HIGH); digitalWrite(motor3pin2, LOW);
  digitalWrite(motor4pin1, HIGH); digitalWrite(motor4pin2, LOW);
}

/**
 * Turn right
 * Right-side wheels drive forward; left-side wheels stop.
 * The wheelchair pivots to the right.
 */
void right() {
  // Right-side motors: forward
  digitalWrite(motor1pin1, HIGH); digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, HIGH); digitalWrite(motor2pin2, LOW);
  // Left-side motors: stop
  digitalWrite(motor3pin1, LOW);  digitalWrite(motor3pin2, LOW);
  digitalWrite(motor4pin1, LOW);  digitalWrite(motor4pin2, LOW);
}
