# FisherNet
November 27, 2021

A search and rescue transponder device targeted for the use of Filipino fisherfolk. Works on the ESP32 microcontroller with LoRa and GPS modules.

In fulfillment of the final project requirement for CMSC 205: Data Communications and Networking
 
**Faculty-in-Charge**

Gulshan Vasson
 
**Team Members:**
* Adrian Arguelles
* Jasper Francisco
* Karla Malla
* Alexander Santos
* Ali Seanard

## How to Install
1. Download the repo as a zip file from GitHub.com, or the `git clone` if you git installed.
2. Open the `FisherNet\FisherNet.ino` file in the Arduino IDE.
3. Uncomment LILYGO or EGIZMO depending on the device you are using.
4. Update the `#define LORA_FREQUENCY` in the **Device Setup** section. By default, the code uses the 433MHz frequency for the LoRa radio. If you are on a different frequency, change the `433.0` value.
5. Plug in your device via USB.
6. Click on the Upload button.