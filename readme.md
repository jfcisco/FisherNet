# safetyNET
January 8, 2021

A search and rescue transponder device targeted for the use of Filipino fisherfolk and other stakeholders. Works on the ESP32 microcontroller with LoRa and GPS modules.

In fulfillment of the final project requirement for CMSC 205: Data Communications and Networking

This project was formerly known as fisherNET. The project's renaming was done in view of an expansion of the project's original vision.
 
**Faculty-in-Charge**

Gulshan Vasson
 
**Team Members:**
* Adrian Arguelles
* Jasper Francisco
* Karla Malla
* Alexander Santos
* Ali Seanard

## How to Install
1. Download the [repo as a zip file](https://github.com/jfcisco/FisherNet/archive/refs/heads/main.zip) from GitHub.com, or the `git clone` command if you have `git` installed on your PC.
1. Install the required libraries in your Arduino IDE. See the section below for the list of required libraries.
2. Open the `SafetyNet\SafetyNet.ino` file in the Arduino IDE.
5. Plug in your device via USB.
6. Click on the Upload button.
1. Set your device's unique node address and frequency using the Serial Monitor. The following commands can be entered
    - Enter `SET ADD <node_address>` to set the device's unique address.
    - Enter `GET ADD` to get the set address for the current device.
    - Enter `SET FRQ <frequency>` to set the LoRa frequency in MHz to be used for the device.
    - Enter `GET FRQ` to get the set frequency for the current device.

## Required Libraries
- Adafruit SSD1306
- TinyGPS++
- ESPSoftwareSerial
- LoRa
- OneButton
- RadioHead
