// This code covers Sending Distress Cancellation and Receiving 
void CancelDistress_setup() {
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(WHITE);

  // Add click handlers - only for the confirmed Rescuer device(s) that need(s) to acknowledge it
  button1.attachClick(doNothing);         // They have to long press to acknowledge
  button1.attachDoubleClick(doNothing);   // They have to long press to acknowledge
  button1.attachLongPressStart(acknowledgeCancellation);
  button1.setPressTicks(300); //time to distinguish click vs long press
  button1.setClickTicks(500); //time to distinguish click vs double click
  
  button2.attachClick(doNothing);
  button2.attachDoubleClick(doNothing);
  button2.attachLongPressStart(doNothing);
  button3.attachClick(doNothing);
  button3.attachDoubleClick(doNothing);
  button3.attachLongPressStart(doNothing);
  button4.attachClick(doNothing);
  button4.attachDoubleClick(doNothing);
  button4.attachLongPressStart(doNothing);
  button5.attachClick(doNothing);
  button5.attachDoubleClick(doNothing);
  button5.attachLongPressStart(doNothing);
  
}

void CancelDistress_loop() {
  if (!isRescuer) {
    broadcastCancel();
    delay(2000);
    // Ending should be back to menu
    changeProgramState(DEFAULT_MENU);
  }
  else {
    oled.clearDisplay();
    oled.setCursor(0, 10);
    oled.println("DISTRESS CANCELLATION received");
    oled.print("from BOAT ");
    oled.print(distData.address);
    oled.println("\nLONG PRESS BTN 1");
    oled.println("to acknowledge.");
    oled.display();
  }
}

void broadcastCancel(){  
  // Broadcasts cancellations to all boats
  // Rescuer device acknowledgement required -> See Rescuer portion -> then acknowledge Cancellation below
  float gpsLat = 999.0;     //random
  float gpsLong = 999.0;    //random
  AlertLevel currentAlertLevel = ALERT_TECHNICAL; //random
  bool cancelFlag = true;      // true because it is cancellation
  unsigned long currentTime = millis();

  // Broadcast a distress signal
  if (mesh.sendDistressSignal(gpsLat, gpsLong, currentAlertLevel, cancelFlag)){
    // Print debugging
    Serial.printf("Distress cancellation signal sent at %lu\n", currentTime);
    Serial.print(mesh.getDistressSignal().cancelFlag);
  }

}

// ACS: Message to be received when broadcasting distress signal cancellation - no response needed.
// This is for devices other than confirmed rescuers who will get a different message and need to acknowledge.
void receiveCancelBroadcast() {
  oled.clearDisplay();
  oled.setTextSize(1);          // text size
  oled.setTextColor(WHITE);     // text color
  oled.setCursor(0, 10);
  oled.println("DISTRESS CANCELLATION");
  oled.println("received from");
  oled.print("BOAT");
  oled.print(receivedSignal.address);
  oled.println("\nReturning to main menu.");
  oled.display();
  delay(3000);
  changeProgramState(DEFAULT_MENU);
}

void acknowledgeCancellation() {
  oled.clearDisplay();
  oled.setCursor(0, 10);
  oled.println("DISTRESS CANCELLATION");
  oled.print("from BOAT ");
  oled.print(distData.address);
  oled.print("\nacknowledged.");
  oled.println("\nReturning to main menu.");
  oled.display();
  delay(3000);
  changeProgramState(DEFAULT_MENU);
}
