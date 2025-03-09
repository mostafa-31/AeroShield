#include <BluetoothSerial.h>
#include <Wire.h>
#include <Keypad_I2C.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>

// ================== Radar Section ==================
BluetoothSerial SerialBT;

// Radar Servos
Servo scanningServo;
Servo trackingServo1;
Servo trackingServo2;

// Ultrasonic pins
#define TRIG_PIN1 5
#define ECHO_PIN1 18
#define TRIG_PIN2 23
#define ECHO_PIN2 19

// Servo pins
#define SCANNING_SERVO_PIN 32
#define TRACKING_SERVO1_PIN 33
#define TRACKING_SERVO2_PIN 25

// Buzzer pin
#define BUZZER_PIN 15

// Radar parameters
const float DISTANCE_THRESHOLD = 10.0;
int currentTrackingAngle1 = -1;
int currentTrackingAngle2 = -1;
int scanAngle = 0;
int scanDirection = 1;  // 1 for increasing, -1 for decreasing
unsigned long lastScanUpdate = 0;

// ================== Door Lock Section ==================
// I2C Addresses
#define KEYPAD_I2C_ADDRESS 0x20
#define LCD_ADDRESS 0x27

// LCD Configuration
LiquidCrystal_I2C lcd(LCD_ADDRESS, 16, 2);

// Door Lock Servo
Servo lockServo;
#define SERVO_PIN 13
int lockedPos = 0;
int unlockedPos = 180;

// Keypad setup
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {0, 1, 2, 3};
byte colPins[COLS] = {4, 5, 6, 7};
Keypad_I2C keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS, KEYPAD_I2C_ADDRESS);

// Lock parameters
String password = "1234AB";
String inputPassword = "";
unsigned long lockTime = 0;
bool isLocked = true;

// ================== Shared Setup ==================
void setup() {
  // Radar initialization
  Serial.begin(115200);
  SerialBT.begin("RadarSystem");
  
  scanningServo.attach(SCANNING_SERVO_PIN);
  trackingServo1.attach(TRACKING_SERVO1_PIN);
  trackingServo2.attach(TRACKING_SERVO2_PIN);

  pinMode(TRIG_PIN1, OUTPUT);
  pinMode(ECHO_PIN1, INPUT);
  pinMode(TRIG_PIN2, OUTPUT);
  pinMode(ECHO_PIN2, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Door Lock initialization
  Wire.begin(21, 22);
  keypad.begin();
  
  lcd.init();
  lcd.backlight();
  lcd.print("Enter Password:");
  lcd.setCursor(0, 1);

  lockServo.attach(SERVO_PIN);
  lockDoor();
}

// ================== Main Loop ==================
void loop() {
  // Radar scanning (non-blocking)
  if(millis() - lastScanUpdate >= 100) {
    updateRadarScan();
    lastScanUpdate = millis();
  }

  // Door lock handling
  handleDoorLock();
}

// ================== Radar Functions ==================
void updateRadarScan() {
  scanningServo.write(scanAngle);
  processUltrasonicSensors(scanAngle, 0);  // Sensor 1
  processUltrasonicSensors(scanAngle, 1);  // Sensor 2

  // Update scan angle
  scanAngle += scanDirection * 5;
  
  // Reverse direction at limits
  if(scanAngle >= 180 || scanAngle <= 0) {
    scanDirection *= -1;
  }
}

void processUltrasonicSensors(int angle, int sensorNo) {
  float distance = (sensorNo == 0) ? measureDistance(TRIG_PIN1, ECHO_PIN1) 
                                  : measureDistance(TRIG_PIN2, ECHO_PIN2);

  if(distance > 0 && distance < DISTANCE_THRESHOLD) {
    int finalAngle = sensorNo == 0 ? angle : (angle + 180);
    notifyAndReact(sensorNo + 1, distance, finalAngle);
    sendToProcessing(sensorNo + 1, distance, finalAngle);

    // Update tracking servos
    if(sensorNo == 0) {
      currentTrackingAngle1 = finalAngle;
      trackingServo1.write(finalAngle);
    } else {
      currentTrackingAngle2 = finalAngle;
      trackingServo2.write(finalAngle - 180);
    }
  } else {
    if(sensorNo == 0) currentTrackingAngle1 = -1;
    else currentTrackingAngle2 = -1;
  }
}

float measureDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);
  return duration > 0 ? duration * 0.034 / 2 : -1;
}

void notifyAndReact(int sensorNo, float distance, int angle) {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(50);
  digitalWrite(BUZZER_PIN, LOW);

  String message = "🚨 ALERT: Sensor " + String(sensorNo) + "\n";
  message += "Distance: " + String(distance) + " cm\n";
  message += "Angle: " + String(angle) + "°\n";
  SerialBT.println(message);
}

void sendToProcessing(int sensorNo, float distance, int angle) {
  Serial.printf("Sensor%d: %.1fcm @ %d°\n", sensorNo, distance, angle);
}

// ================== Door Lock Functions ==================
void handleDoorLock() {
  char key = keypad.getKey();
  
  if(key) handleKeyPress(key);
  
  if(!isLocked && millis() - lockTime >= 5000) {
    lockDoor();
  }
}

void handleKeyPress(char key) {
  if(key == '*') {
    checkPassword();
  }
  else if(key == '#') {
    deleteLastChar();
  }
  else if(inputPassword.length() < 6) {
    inputPassword += key;
    updateDisplay();
  }
}

void checkPassword() {
  if(inputPassword == password) {
    unlockDoor();
  } else {
    lcd.clear();
    lcd.print("Wrong Password!");
    delay(2000);
    lockDoor();
  }
}

void unlockDoor() {
  isLocked = false;
  lockTime = millis();
  lockServo.write(unlockedPos);
  lcd.clear();
  lcd.print("Access Granted!");
  delay(1000);
  lcd.clear();
  lcd.print("Door Unlocked");
}

void lockDoor() {
  isLocked = true;
  lockServo.write(lockedPos);
  inputPassword = "";
  lcd.clear();
  lcd.print("Enter Password:");
  lcd.setCursor(0, 1);
}

void deleteLastChar() {
  if(inputPassword.length() > 0) {
    inputPassword.remove(inputPassword.length() - 1);
    updateDisplay();
  }
}

void updateDisplay() {
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print(inputPassword);
}