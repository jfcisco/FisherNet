// This code displays the default screen showing the MENU on the OLED for all types of devices.

bool distRec; //rescuer - distress signal received

// ALI's EDITS: SINGLE BUTTON EDITION
// Declare Variable for the slideshow menu
byte optionSelected; 

void DefaultMenu_setup() {
  // Initialize optionSelected to 0
  optionSelected = 0; 
  
  //Display Main Menu Slideshow Version
  mainMenu();

  //call BTN_1 Click functions
  button1.attachClick(optionSelection);
  // ACS: IDK if it matters whether they double click or long press
  // ACS: If it does, then we can just create an error handler, the doNothing function, or create a new function altogether :)
  button1.attachDoubleClick(confirmClick);
  button1.attachLongPressStart(confirmClick);
  button1.setPressTicks(300); //time to distinguish click vs long press
  button1.setClickTicks(500); //time to distinguish click vs double click
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
    default:
      oled.print(optionSelected+1);
      oled.setTextSize(2);
      oled.println("\nTechnical\nTeknikal");
      currentAlertLevel = ALERT_TECHNICAL;
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

// ACS: CLICK - Move along the menus
void optionSelection() {
  if (optionSelected > 3) {  
    optionSelected=0;
  }
  else {
    optionSelected++;
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
/* ORIGINAL CODE
  //print menu
  oled.print("Your Boat No: ");  // changed from "Address" for users' better understanding
  oled.print(mesh.getAddress());
  oled.println("\n\nMENU: ");
  oled.println(" 1=Technical/Teknikal");
  oled.println(" 2=Hostile/Pirata");
  oled.println(" 3=Sinking/Lumulubog");
  oled.println(" 4=Medical/Medikal");
  oled.println(" 5=Others/Iba pa");
*/
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

/* ORIGINAL CODE
//CLICK functions (ACS: OK, maybe this part isn't original code, just made it a bit more efficient >_<)
void buttonSelect() {
  mainmenu();   //wake screen
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

END OF ORIGINAL CODE */
