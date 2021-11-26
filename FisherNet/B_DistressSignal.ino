#define DISTRESS_SIGNAL_INTERVAL 30000

unsigned long timeLastSignalSent = 0;
DistressResponse lastResponse;
bool receivedResponse;
// Did the user double click while in this state?
bool didError = false;

DistressResponse dummyRes = {
  {
    DISTRESS_RESPONSE
  },
  3, // address
  14.6760, // gpsLat
  121.1050 // gpsLng
};

void DistressSignal_setup(AlertLevel al) {
  receivedResponse = false;
  currentAlertLevel = al;
  // Set OLED color to white
  oled.setTextColor(WHITE);

  // Setup button click code
  // TODO: Move cancellation logic here
  //call BTN_1 Click functions
  button1.attachClick(handleClick);
  button1.attachDoubleClick(handleConfirm);
  button1.attachLongPressStart(BTN_1_back);
  button1.setPressTicks(300); //time to distinguish click vs long press
  button1.setClickTicks(500); //time to distinguish click vs double click

  //call BTN_2 Click functions
  button2.attachClick(handleClick);
  button2.attachDoubleClick(handleConfirm);
  button2.attachLongPressStart(BTN_2_back);
  button2.setPressTicks(300); //time to distinguish click vs long press
  button2.setClickTicks(500); //time to distinguish click vs double click

  //call BTN_3 Click functions
  button3.attachClick(handleClick);
  button3.attachDoubleClick(handleConfirm);
  button3.attachLongPressStart(BTN_3_back);
  button3.setPressTicks(300); //time to distinguish click vs long press
  button3.setClickTicks(500); //time to distinguish click vs double click

  //call BTN_4 Click functions
  button4.attachClick(handleClick);
  button4.attachDoubleClick(handleConfirm);
  button4.attachLongPressStart(BTN_4_back);
  button4.setPressTicks(300); //time to distinguish click vs long press
  button4.setClickTicks(500); //time to distinguish click vs double click

  //call BTN_5 Click functions
  button5.attachClick(handleClick);
  button5.attachDoubleClick(handleConfirm);
  button5.attachLongPressStart(BTN_5_back);
  button5.setPressTicks(300); //time to distinguish click vs long press
  button5.setClickTicks(500); //time to distinguish click vs double click

  // Uncomment for testing display
  // lastResponse = dummyRes;
}

void handleClick() {
  return;
}

void handleConfirm() {
  // Trigger error message
  didError = true;
}

//LONG PRESS FUNCTIONS
void BTN_1_back() {
  cancelmessage();
  delay(2000);
  changeProgramState(DEFAULT_MENU);
}

void BTN_2_back() {
  cancelmessage();
  delay(2000);
  changeProgramState(DEFAULT_MENU);
}

void BTN_3_back() {
  cancelmessage();
  delay(2000);
  changeProgramState(DEFAULT_MENU);
}

void BTN_4_back() {
  cancelmessage();
  delay(2000);
  changeProgramState(DEFAULT_MENU);
}

void BTN_5_back() {
  cancelmessage();
  delay(2000);
  changeProgramState(DEFAULT_MENU);
}

void DistressSignal_loop() {
  // When did we last broadcast?
  // If more than 30 seconds passed, we can broadcast again
  unsigned long currentTime = millis();
  if (currentTime - timeLastSignalSent >= DISTRESS_SIGNAL_INTERVAL)
  {
    // Retrieve data to be sent (e.g., GPS, Alert Level)
    float gpsLat;
    float gpsLong;
    // Check if GPS data is valid
    if (gps.location.isValid())
    {
      gpsLat = gps.location.lat();
      gpsLong = gps.location.lng();
      Serial.printf("Valid: GPS (%f, %f)\n", gps.location.lat(), gps.location.lng());
    }
    else
    {
      // Scenario when gps has no valid gps data
      gpsLat = 999.0;
      gpsLong = 999.0;
      Serial.printf("Invalid: GPS (%f, %f)\n", gps.location.lat(), gps.location.lng());
    }

    // Broadcast a distress signal
    if (mesh.sendDistressSignal(gpsLat, gpsLong, currentAlertLevel))
    {
      // Print debugging
      Serial.printf("Distress signal sent at %lu\n", currentTime);
      timeLastSignalSent = currentTime;
    }
  }

  // While we are also in distress, listen for any distress reponse for us
  if (mesh.listenForDistressResponse())
  {
    lastResponse = mesh.getResponse();
    receivedResponse = true;

    Serial.println("Received response");
    Serial.printf("Responder Number: %u\n", lastResponse.address);
    Serial.printf("GPS Lat: %f\n", lastResponse.address);
    Serial.printf("GPS Long: %f\n", lastResponse.address);
  }

  showRescueeMenu();
}

void showRescueeMenu() {
  oled.clearDisplay();

  if (!didError) {
    // No error occurred so we can draw the SOS signal
    // Display the beaming circle
    int phase = (millis() / 1000) % 3;
    drawBeamingCircle(phase);
    
    // Continuously print out response if there is one received
    if (receivedResponse) {
      displayLastResponseInfo(lastResponse);
    }
    oled.display();
  }
  else {
    // Show the error message
    oled.setTextSize(1);          // text size
    oled.setTextColor(WHITE);     // text color
    oled.setCursor(0, 10);        // position to display
    //print menu
    oled.println("Long press any button to cancel current distress signal and back to main menu");
    oled.display();
    delay(2000);
    didError = false;
  }
}

void displayLastResponseInfo(DistressResponse res) {
  // Assumption: Only data of last responder will appear
  oled.setCursor(40, 30);
  oled.setTextSize(1);
  oled.print("RES: ");
  oled.print(res.address, DEC);

  oled.setCursor(35, 40);
  // If both rescuee and rescuer have valid coordinates, display the distance between them
  if (gps.location.isValid() && isValidGps(res.gpsLat, res.gpsLong)) {
    double distanceInMeters = TinyGPSPlus::distanceBetween(
                                gps.location.lat(),
                                gps.location.lng(),
                                res.gpsLat,
                                res.gpsLong);

    // And then, show the distance
    oled.print("DIS: ");
    oled.print(distanceInMeters, 2);
    oled.print("m");
  }
  else {
    oled.print("DIS: N/A");
  }
}

// Draws the beaming circle
#define CIRCLE_CENTER_X 63
#define CIRCLE_CENTER_Y 31
void drawBeamingCircle(int phase)
{
  switch (phase)
  {
    case 2:
      oled.fillCircle(CIRCLE_CENTER_X, CIRCLE_CENTER_Y, 65, WHITE);
      oled.fillCircle(CIRCLE_CENTER_X, CIRCLE_CENTER_Y, 55, BLACK);
    case 1:
      oled.fillCircle(CIRCLE_CENTER_X, CIRCLE_CENTER_Y, 45, WHITE);
      oled.fillCircle(CIRCLE_CENTER_X, CIRCLE_CENTER_Y, 35, BLACK);
  }

  oled.setCursor(40, 10);
  oled.setTextSize(2);
  oled.print("SOS");
  oled.print(currentAlertLevel + 1, DEC);
}
