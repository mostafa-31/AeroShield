# AeroShield - Smart Air Defense System

## Overview
**AeroShield** is an advanced Smart Air Defense System built with an ESP32 microcontroller. It features radar-based object tracking, signal jamming using D.Aouth, and a secure password-protected door lock. The radar system employs ultrasonic sensors for object detection and sends real-time alerts via Bluetooth, while the access control system ensures restricted entry through an I2C keypad and servo motor.

## Features
- **Radar-Based Object Tracking**: Detects unregistered drones or air objects using ultrasonic sensors.
- **Signal Jamming**: Uses D.Aouth via ESP32 to interfere with unauthorized signals.
- **Bluetooth Alerts**: Sends real-time notifications about detected objects.
- **Password-Protected Door Lock**: Controlled using an I2C keypad and servo motor for secure access.
- **LCD Display**: Provides feedback on the security system status.
- **Buzzer Alerts**: Notifies when an intruder or unauthorized object is detected.

## Components Used
### Hardware
- ESP32
- Ultrasonic Sensors (HC-SR04)
- Servo Motors
- Bluetooth Module (Integrated in ESP32)
- I2C Keypad
- I2C LCD Display
- Buzzer
- Jumper Wires

### Software & Libraries
- **Arduino IDE**
- **BluetoothSerial.h** (For Bluetooth communication)
- **Wire.h** (For I2C communication)
- **Keypad_I2C.h** (For keypad handling)
- **LiquidCrystal_I2C.h** (For LCD display)
- **ESP32Servo.h** (For controlling servo motors)

## Installation & Setup
1. Clone this repository:
   ```bash
   git clone https://github.com/mostafa-31/AeroShield.git
   ```
2. Open the project in **Arduino IDE**.
3. Install the required libraries from the Arduino Library Manager.
4. Connect the ESP32 and upload the code.
5. Ensure all hardware connections are properly configured.

## Usage
- Power on the system and allow it to initialize.
- The radar system will begin scanning for objects.
- If an object is detected within the set threshold, an alert will be sent via Bluetooth.
- The signal jammer activates when an unauthorized signal is detected.
- Enter the correct password on the keypad to unlock the door; after 5 seconds, it will automatically relock.

## Future Improvements
- Integration of AI-based object classification for enhanced detection.
- Adding GSM module for SMS alerts.
- Enhancing the UI with a web-based monitoring dashboard.

## License
This project is open-source and available under the MIT License.
