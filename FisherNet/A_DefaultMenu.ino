bool distRec; //rescuer - distress signal received

void DefaultMenu_setup() {
  //Display Main Menu
  mainmenu();

  //call BTN_1 Click functions
  button1.attachClick(BTN_1_select);
  button1.attachDoubleClick(BTN_1_confirm);
  button1.attachLongPressStart(BTN_1_back);
  button1.setPressTicks(300); //time to distinguish click vs long press
  button1.setClickTicks(500); //time to distinguish click vs double click

  //call BTN_2 Click functions
  button2.attachClick(BTN_2_select);
  button2.attachDoubleClick(BTN_2_confirm);
  button2.attachLongPressStart(BTN_2_back);
  button2.setPressTicks(300); //time to distinguish click vs long press
  button2.setClickTicks(500); //time to distinguish click vs double click

  //call BTN_3 Click functions
  button3.attachClick(BTN_3_select);
  button3.attachDoubleClick(BTN_3_confirm);
  button3.attachLongPressStart(BTN_3_back);
  button3.setPressTicks(300); //time to distinguish click vs long press
  button3.setClickTicks(500); //time to distinguish click vs double click

  //call BTN_4 Click functions
  button4.attachClick(BTN_4_select);
  button4.attachDoubleClick(BTN_4_confirm);
  button4.attachLongPressStart(BTN_4_back);
  button4.setPressTicks(300); //time to distinguish click vs long press
  button4.setClickTicks(500); //time to distinguish click vs double click

  //call BTN_5 Click functions
  button5.attachClick(BTN_5_select);
  button5.attachDoubleClick(BTN_5_confirm);
  button5.attachLongPressStart(BTN_5_back);
  button5.setPressTicks(300); //time to distinguish click vs long press
  button5.setClickTicks(500); //time to distinguish click vs double click
}

void DefaultMenu_loop() {
  distRec = mesh.listenForDistressSignal();
  if (distRec) {
    receivedSignal = mesh.getDistressSignal();
    // Set distress signal variable to received signal data
    // Change state to RESCUER_MENU
    changeProgramState(RESCUER_MENU);
  }
}

//MENU FUNCTIONS
void mainmenu() {
  oled.clearDisplay();
  oled.setTextSize(1);          // text size
  oled.setTextColor(WHITE);     // text color
  oled.setCursor(0, 10);        // position to display
  //print menu
  oled.println("1) Technical/Teknikal");
  oled.println("2) Hostile/Pirata");
  oled.println("3) Sinking/Lumulubog");
  oled.println("4) Medical/Medikal");
  oled.println("5) Others/Iba pa");
  oled.print("Address: ");
  oled.print(mesh.getAddress());
  oled.display();
}

void errormessage() {
  oled.clearDisplay();
  oled.setTextSize(1);          // text size
  oled.setTextColor(WHITE);     // text color
  oled.setCursor(0, 10);        // position to display
  //print menu
  oled.println("Long press any button to cancel current distress signal and back to main menu");
  oled.display();
}

// Replaced by B_DistressSignal program
//void confirmationmessage() {
//  oled.clearDisplay(); // clear display
//  delay(100); // wait for initializing
//  oled.setTextSize(1);          // text size
//  oled.setTextColor(WHITE);     // text color
//  oled.setCursor(0, 10);        // position to display
//
//  switch (distresssignal) { //modifying messages and alert notifications based on button clicked.
//    case 0:
//      oled.println("Select a distress signal first");
//      oled.display();
//      break;
//
//    case 1:
//      oled.println("Technical emergency relayed");
//      oled.println("\nLong press any key to main menu");
//      oled.display();
//      Serial.println("Distress signal 1");
//      break;
//
//    case 2:
//      oled.println("Fleet alerted of hostile party");
//      oled.println("\nLong press any key to main menu");
//      oled.display();
//      Serial.println("Distress signal 2");
//      break;
//
//    case 3:
//      oled.println("Sinking SOS relayed");
//      oled.println("\nLong press any key to main menu");
//      oled.display();
//      Serial.println("Distress signal 3");
//      break;
//
//    case 4:
//      oled.println("Medical emergency relayed");
//      oled.println("\nLong press any key to main menu");
//      oled.display();
//      Serial.println("Distress signal 4");
//      break;
//
//    case 5:
//      oled.println("Your distress signal is being relayed to your fleet");
//      oled.println("\nLong press any key to main menu");
//      oled.display();
//      Serial.println("Distress signal 5");
//      break;
//  }
//}

void cancelmessage() {
  oled.clearDisplay(); // clear display
  delay(100); // wait for initializing
  oled.setTextSize(1);          // text size
  oled.setTextColor(WHITE);     // text color
  oled.setCursor(0, 10);        // position to display
  oled.println("Stopping the distress signal.");
  oled.println("\nGoing back to main menu.");
  oled.display();

}

//CLICK functions
void BTN_1_select() {
  mainmenu();   //wake screen
}

void BTN_2_select() {
  mainmenu();  //wake screen
}

void BTN_3_select() {
  mainmenu();  //wake screen
}

void BTN_4_select() {
  mainmenu();  //wake screen
}

void BTN_5_select() {
  mainmenu();  //wake screen
}


//DOUBLE CLICK FUNCTIONS
void BTN_1_confirm() {
  currentAlertLevel = ALERT_TECHNICAL;
  changeProgramState(IN_DISTRESS);
}

void BTN_2_confirm() {
  currentAlertLevel = ALERT_HOSTILE;
  changeProgramState(IN_DISTRESS);
}

void BTN_3_confirm() {
  currentAlertLevel = ALERT_SINKING;
  changeProgramState(IN_DISTRESS);
}

void BTN_4_confirm() {
  currentAlertLevel = ALERT_MEDICAL;
  changeProgramState(IN_DISTRESS);
}

void BTN_5_confirm() {
  currentAlertLevel = ALERT_GENERAL;
  changeProgramState(IN_DISTRESS);
}
