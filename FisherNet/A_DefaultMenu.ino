// This code displays the default screen showing the MENU on the OLED for all types of devices.

bool distRec; //rescuer - distress signal received

// Declare variable for the slideshow menu
byte optionSelected; 

void DefaultMenu_setup() {
  // Initialize optionSelected to 0
  optionSelected = 0; 
  
  //Display Main Menu Slideshow Version
  mainMenu();

  // Setup buttons for this state
  DefaultMenu_setupButtons();
}

void DefaultMenu_loop() {
  oled.clearDisplay();
  oled.setCursor(0,10);
  oled.setTextSize(1);
  oled.print("Option ");

  // ACS: Switch-Case to display the Slideshow Menu
  switch(optionSelected) {   
    // ACS: Maybe we can add a case 0 which is the first screen showing Boat Number, GPS Long, GPS Lat, plus some instructions (click/dbl click/long press).
    // ACS: Made the text bigger because of my poor eyesight. >_<
    case 0:
      oled.print(optionSelected+1);
      oled.setTextSize(2);
      oled.println("\nTechnical\nTeknikal");
      currentAlertLevel = ALERT_TECHNICAL;
      break;   
    case 1:
      currentAlertLevel = ALERT_HOSTILE;
      oled.print(optionSelected+1);
      oled.setTextSize(2);
      oled.println("\nHostile\nPirata");
      break;      
    case 2:
      oled.print(optionSelected+1);
      oled.setTextSize(2);
      currentAlertLevel = ALERT_SINKING;
      oled.println("\nSinking\nLumulubog");
      break;      
    case 3:
      oled.print(optionSelected+1);
      oled.setTextSize(2);
      currentAlertLevel = ALERT_MEDICAL;
      oled.println("\nMedical\nMedikal");
      break;      
    case 4:
      oled.print(optionSelected+1);
      oled.setTextSize(2);
      oled.println("\nOthers\nIba pa");
      currentAlertLevel = ALERT_GENERAL;
      break;
  }
  oled.display();

  // While the menu is showing, keep listening for distress signal
  distRec = mesh.listenForDistressSignal();
  if (distRec) {
    receivedSignal = mesh.getDistressSignal();
    // Set distress signal variable to received signal data
    // ACS: Check first if distress signal is a cancellation based on its cancelFlag
    if (!receivedSignal.cancelFlag) {
      changeProgramState(RESCUER_MENU);
    }
  }
}

// ACS: DOUBLE CLICK/LONG PRESS - Confirm selection
void confirmClick() {
  changeProgramState(IN_DISTRESS);
}

//MENU FUNCTIONS

void mainMenu() {
  oled.clearDisplay();
  oled.setTextSize(1);          // text size
  oled.setTextColor(WHITE);     // text color
  oled.setCursor(0, 0);         // position to display
  oled.display();
}

// ACS:  We may not need this anymore if we don't need to distinguish double click from long press.
void errormessage() {
  oled.clearDisplay();
  oled.setTextSize(1);          // text size
  oled.setTextColor(WHITE);     // text color
  oled.setCursor(0, 10);        // position to display
  //print menu
  oled.println("Long press any button to cancel current distress signal and go back to main menu");
  oled.display();
}

// Time to distinguish click vs long press
#define GENERAL_PRESS_TICKS 300

// Time to distinguish click vs double click
#define GENERAL_CLICK_TICKS 500

// Subroutine to setup the buttons in this state
void DefaultMenu_setupButtons() {
  // Using lambda/anonymous function to set optionSelected
  button1.attachClick([]() {
    optionSelected = 0;
  });
  // ACS: IDK if it matters whether they double click or long press
  // ACS: If it does, then we can just create an error handler, the doNothing function, or create a new function altogether :)
  button1.attachDoubleClick(confirmClick);
  button1.attachLongPressStart(confirmClick);
  button1.setPressTicks(GENERAL_PRESS_TICKS);
  button1.setClickTicks(GENERAL_CLICK_TICKS); 

  button2.attachClick([]() {
    optionSelected = 1;
  });
  button2.attachDoubleClick(confirmClick);
  button2.attachLongPressStart(confirmClick);
  button2.setPressTicks(GENERAL_PRESS_TICKS);
  button2.setClickTicks(GENERAL_CLICK_TICKS); 

  button3.attachClick([]() {
    optionSelected = 2;
  });
  button3.attachDoubleClick(confirmClick);
  button3.attachLongPressStart(confirmClick);
  button3.setPressTicks(GENERAL_PRESS_TICKS);
  button3.setClickTicks(GENERAL_CLICK_TICKS);

  button4.attachClick([]() {
    optionSelected = 3;
  });
  button4.attachDoubleClick(confirmClick);
  button4.attachLongPressStart(confirmClick);
  button4.setPressTicks(GENERAL_PRESS_TICKS);
  button4.setClickTicks(GENERAL_CLICK_TICKS); 

  button5.attachClick([]() {
    optionSelected = 4;
  });
  button5.attachDoubleClick(confirmClick);
  button5.attachLongPressStart(confirmClick);
  button5.setPressTicks(GENERAL_PRESS_TICKS);
  button5.setClickTicks(GENERAL_CLICK_TICKS);
}
