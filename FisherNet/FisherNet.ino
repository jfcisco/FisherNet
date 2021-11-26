/**
 * FisherNet v1.0
 * November 27, 2021
 * 
 * In fulfillment of the final project requirement
 * for CMSC 205: Data Communications and Networking
 * 
 * Faculty-in-Charge: 
 *    Gulshan Vasson
 * 
 * Team Members:
 *    Arguelles
 *    Francisco
 *    Malla
 *    Santos
 *    Seanard
 */

// =============
// Device Setup
// =============
//  **IMPORTANT!!** Uncomment the device you are using
#define LILYGO
// #define EGIZMO

// **IMPORTANT!!**
#define NODE_ADDRESS 1 // IMPORTANT: NODE_ADDRESS should be unique per device
#define LORA_FREQUENCY 433.0 // Frequency in MHz. Different for SG!

#include "PinAssignments.h"


// ==================
// Libraries Setup
// ==================
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "TinyGPS++.h"
#include <OneButton.h>
#include <SoftwareSerial.h>
#include "FisherMesh.h"

// Setup a new OneButton pins
OneButton button1(BTN_1, true);
OneButton button2(BTN_2, true);
OneButton button3(BTN_3, true);
OneButton button4(BTN_4, true);
OneButton button5(BTN_5, true);

// Setup OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Setup GPS
SoftwareSerial gpsSerial(GPS_RX, GPS_TX);
TinyGPSPlus gps;
FisherMesh mesh(NODE_ADDRESS, LORA_FREQUENCY);

// =========================
// Program State Definition
// =========================

typedef enum {
  DEFAULT_MENU,
  IN_DISTRESS,
  RESCUER_MENU,
  RESCUE_MODE
}
ProgramState;

// =================
// Main Program
// =================

ProgramState currentState = DEFAULT_MENU;
AlertLevel currentAlertLevel;
DistressSignal receivedSignal;

void DistressSignal_setup(AlertLevel al);

void setup() {
  setupDevice();
  
  // Call the setup function of the current state
  setupState(currentState);
}

void loop() {
  updateButtons();
  updateGps();

  // Call the loop function of the current state
  switch (currentState) {
    case DEFAULT_MENU:
      DefaultMenu_loop();    
      break;
    case IN_DISTRESS:
      DistressSignal_loop();
      break;
    case RESCUER_MENU:
      Rescuer_loop();
      break;
  }
}

// =================
// Helper Functions
// =================

void setupDevice() {
  Serial.begin(115200);
  Serial.println(String(NODE_ADDRESS) + " loading");
  setupOled();
  setupGps();
  
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  if (!mesh.init()) {  
    Serial.println(F("Failed to initialize mesh network"));
    oled.println("Failed to initialize mesh network");
    while (true);
  }
  
  oled.println("Device Setup Success");
  
  oled.setTextSize(2);
  oled.println("Loading...");
  oled.display();
  delay(2000);
}

void setupOled() {
  // Initialize OLED display with address 0x3C for 128x64
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }

  oled.clearDisplay();
  oled.fillRect(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1, WHITE);
  oled.display();
  delay(2000);
  oled.clearDisplay();
}

// Setup GPS serial connection
void setupGps() {
  gpsSerial.begin(9600);
}

// Updates gps with data from the module
void updateGps() {
  while (gpsSerial.available() > 0)
    gps.encode(gpsSerial.read());
}

// Ticks buttons as required by OneButton
void updateButtons() {
  button1.tick();
  button2.tick();
  button3.tick();
  button4.tick();
  button5.tick();
}

// Transitions from the current program state to a new one
void changeProgramState(ProgramState newState) {
  currentState = newState;
  setupState(newState);
}

// Calls setup method of a given program state
void setupState(ProgramState state) {
  switch (state) {
    case DEFAULT_MENU:
      DefaultMenu_setup();
      break;
    case IN_DISTRESS:
      DistressSignal_setup(currentAlertLevel);
      break;
    case RESCUER_MENU:
      Rescuer_setup(receivedSignal);
      break;
  }
}

// Checks if the given lat and long are valid
bool isValidGps(float latitude, float longitude) {
  if (latitude < -90.0 || latitude > 90.0) return false;
  if (longitude < - 180.0 || longitude > 180.0) return false;
  return true;
}
