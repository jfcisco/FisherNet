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
Use git to clone the repo or download the repo as a zip file from GitHub.com.

Make sure to uncomment LILYGO or EGIZMO depending on the device you are using.

By default, the code uses the 433MHz frequency for the LoRa radio. If you are on a different frequency, kindly update the `#define LORA_FREQUENCY` in the **Device Setup** section.
