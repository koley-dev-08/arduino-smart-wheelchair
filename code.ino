#include <Keypad.h>
#include "Wire.h"       
#include "I2Cdev.h"     
#include "MPU6050.h"
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

// The TinyGPS++ object
TinyGPSPlus gps;

#define echoPin A15  // attach pin A15 Arduino to pin Echo of HC-SR04
#define trigPin A14  // attach pin A14 Arduino to pin Trig of HC-SR04                                          \

#define buzzer A7  //attach to buzzer

//gps module
String lati = "";
String longi = "";

long duration;
int distance;

//accelerometer
MPU6050 mpu;
int16_t ax, ay, az;
int16_t gx, gy, gz;

struct MyData {
  byte X;
  byte Y;
  byte Z;
};

MyData data;

//keypad module
const byte ROWS = 4;  //four rows
const byte COLS = 4;  //four columns

char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

byte colPins[COLS] = { 31, 32, 33, 34 };  //connect to the row pinouts of the keypad
byte rowPins[ROWS] = { 9, 10, 11, 12 };  //connect to the column pinouts of the keypad

//Create an object of keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

//motor inputs
int motor1pin1 = 36;
int motor1pin2 = 38;

int motor2pin1 = 40;
int motor2pin2 = 42;

int motor3pin1 = 52;
int motor3pin2 = 50;

int motor4pin1 = 48;
int motor4pin2 = 46;

//control motor speed
int EN1 = 7;  
int EN2 = 6;  
int EN3 = 8;  


int xVal = 340;
int yVal = 338;
int xPin = A1;
int yPin = A2;

int mode;

//gsm module
#define rxPin 2
#define txPin 3
SoftwareSerial sim800L(rxPin,txPin); 




void setup() {
  Serial1.begin(9600);
  Serial.print("Mega up");
  pinMode(buzzer, OUTPUT);
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(motor2pin1, OUTPUT);
  pinMode(motor2pin2, OUTPUT);
  pinMode(motor3pin1, OUTPUT);
  pinMode(motor3pin2, OUTPUT);
  pinMode(motor4pin1, OUTPUT);
  pinMode(motor4pin2, OUTPUT);
  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT);
  pinMode(EN3, OUTPUT);
  
  Serial.begin(9600);
  Wire.begin();
  mpu.initialize();
  pinMode(LED_BUILTIN, OUTPUT);


  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  sim800L.begin(9600);

  Serial.println("Initializing...");
  
  sim800L.println("AT");
  waitForResponse();

  sim800L.println("ATE1");
  waitForResponse();

  sim800L.println("AT+CMGF=1");
  waitForResponse();

  sim800L.println("AT+CNMI=1,2,0,0,0");
  waitForResponse();
}

void loop() {
  analogWrite(EN1, 100);
  analogWrite(EN2, 200);
  analogWrite(EN3, 200);
  
  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2); // wait for 2 ms to avoid
  digitalWrite(trigPin, HIGH); // turn on the Trigger to generate pulse
  delayMicroseconds(10); // keep the trigger "ON" for 10 ms to generate
  digitalWrite(trigPin,LOW); // Turn off the pulse trigger to stop
                       // pulse generation

    // If pulse reached the receiver echoPin
    // become high Then pulseIn() returns the
    // time taken by the pulse to reach the
    // receiver

    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.0344 / 2; // Expression to calculate
                                 // distance using time

    Serial.print("Distance: ");
    Serial.print(distance); // Print the output in serial monitor
    Serial.println(" cm");
    delay(100);
  

  readKeypad(); // Handle mode switching

  if (mode == 1) {
    joystickControl();
  } else if (mode == 2) {
    gestureControl();
  } else if (mode == 3){
    stop();}
    else {
    stop();
  }

  

  if(distance < 30)
  {
    digitalWrite(buzzer, HIGH);
    delay(1000);
    digitalWrite(buzzer, LOW);
    delay(500);
  }
  
}

void joystickControl()
{
  xVal = analogRead(xPin);
  yVal = analogRead(yPin);
  Serial.print("X : ");
  Serial.println(xVal);
  Serial.print("Y : ");
  Serial.println(yVal);
  if (xVal >= 345 && distance >= 30) 
  {
    forward();
  }
  if (xVal <= 335 && distance >= 30) {
    backward();
  }
  if (yVal >= 343 && distance >= 30) {
    right();
  }
  if (yVal <= 333 && distance >= 30) {
    left();
  }
  if (xVal < 345 && xVal > 335 && yVal < 343 && yVal > 333 || distance < 30) {
    stop();
  } 
}

void gestureControl()
{
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  data.X = map(ax, -17000, 17000, 0, 255 ); // X axis data
  data.Y = map(ay, -17000, 17000, 0, 255); 
  data.Z = map(az, -17000, 17000, 0, 255);  // Y axis data
  Serial.print("Axis X = ");
  Serial.print(data.X);
  Serial.print("  ");
  Serial.print("Axis Y = ");
  Serial.print(data.Y);
  Serial.print("  ");
  Serial.print("Axis Z  = ");
  Serial.println(data.Z);
  if (data.Y >=180 && data.X <= 160 && distance >= 30)
  {
    forward();
  }
  if (data.Y <=100 && data.X <= 160 && distance >= 30)
  {
    backward();
  }
  if (data.X >=190 && data.Y <= 170 && distance >= 30)
  {
    right();
  }
  if (data.X <=100 && data.Y <=170 && distance >= 30)
  {
    left();
  }
  if (data.X < 190 && data.X > 100 && data.Y > 100 && data.Y < 180 || distance < 30)
  {
    stop();
  }

}

void readKeypad() {
  char key = keypad.getKey();
  if (key) {
    Serial.print("Key Pressed: "); Serial.println(key);
    if (key == '6') {
      mode = 1;
      Serial.println("MODE: JOYSTICK CONTROL");
      delay(200); // debounce
    } if (key == '5') {
      mode = 2;
      Serial.println("MODE: GESTURE CONTROL");
      delay(200); // debounce
    } if (key == '2'){
      make_call();
    } if (key == '1'){
       send_sms();
    } if (key == '3'){
      mode = 3;
      Serial.println("MODE: STOP");
    }
}
}
void send_sms(){
  if (Serial1.available())
   {
      gps.encode(Serial1.read());
      
      lati = gps.location.lat(), 6;
       
      longi = gps.location.lng(), 6;
   }
  sim800L.print("AT+CMGS=\"+919932476089\"\r");
  waitForResponse();
  Serial.print("SMS");
  sim800L.print("Emergency Alert! The user of the smart wheelchair needs assistance. Location: https://maps.google.com/?q=" + lati + "," + longi + ". Please respond immediately.");
  sim800L.write(0x1A);
  waitForResponse();
}

void make_call(){
  sim800L.println("ATD+919932476089;");
  
  waitForResponse();
  Serial.print("CALL");
}

void waitForResponse(){
  delay(1000);
  while(sim800L.available()){
    Serial.println(sim800L.readString());
  }
  sim800L.read();
}

void forward() {
  
    digitalWrite(motor1pin1, HIGH);
    digitalWrite(motor1pin2, LOW);
    digitalWrite(motor2pin1, HIGH);
    digitalWrite(motor2pin2, LOW);
    digitalWrite(motor3pin1, HIGH);
    digitalWrite(motor3pin2, LOW);
    digitalWrite(motor4pin1, HIGH);
    digitalWrite(motor4pin2, LOW);
  }

void backward() {
  
    digitalWrite(motor1pin1, LOW);
    digitalWrite(motor1pin2, HIGH);
    digitalWrite(motor2pin1, LOW);
    digitalWrite(motor2pin2, HIGH);
    digitalWrite(motor3pin1, LOW);
    digitalWrite(motor3pin2, HIGH);
    digitalWrite(motor4pin1, LOW);
    digitalWrite(motor4pin2, HIGH);
  
}

void stop() {
    digitalWrite(motor1pin1, LOW);
    digitalWrite(motor1pin2, LOW);
    digitalWrite(motor2pin1, LOW);
    digitalWrite(motor2pin2, LOW);
    digitalWrite(motor3pin1, LOW);
    digitalWrite(motor3pin2, LOW);
    digitalWrite(motor4pin1, LOW);
    digitalWrite(motor4pin2, LOW);
  
}

void left() {

    digitalWrite(motor1pin1, LOW);
    digitalWrite(motor1pin2, LOW);
    digitalWrite(motor2pin1, LOW);
    digitalWrite(motor2pin2, LOW);
    digitalWrite(motor3pin1, HIGH);
    digitalWrite(motor3pin2, LOW);
    digitalWrite(motor4pin1, HIGH);
    digitalWrite(motor4pin2, LOW);
  
}

void right() {
  
    digitalWrite(motor1pin1, HIGH);
    digitalWrite(motor1pin2, LOW);
    digitalWrite(motor2pin1, HIGH);
    digitalWrite(motor2pin2, LOW);
    digitalWrite(motor3pin1, LOW);
    digitalWrite(motor3pin2, LOW);
    digitalWrite(motor4pin1, LOW);
    digitalWrite(motor4pin2, LOW);
 
}