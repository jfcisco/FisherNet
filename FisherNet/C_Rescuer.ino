float currLat = 999.0;
float currLong = 999.0;
bool gpsVal = false;
//Distress signal global vars
DistressSignal distData;
unsigned long timeLastResponseSent;

/*  Defines how long the device waits
 *  between consecutive sending of
 *  distress responses to the distressed
 *  vessel/boat.
*/
#define DISTRESS_RESPONSE_INTERVAL 30000

//global situation variables
bool distAcc; //rescuer - distress signal accepted
int situation;
/* Situation variable meaning:
 *  0 default - not in distress and no distress received
 *  1 in distress no response
 *  2 in distress with response
 *  3 distress received - those in distress can't be rescuers
 *  4 distress accepted - to the rescue mode
 *  5 distress signal ignored
 */

enum RescuerScreen {
  DETAILS,
  NAVAIDS
};

RescuerScreen currentScreen = DETAILS;

void Rescuer_setup(DistressSignal distress) {
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  
  // Initialize variables
  distData = distress;
  
  timeLastResponseSent = 0;
  distRec = true; //rescuer - distress signal received
  distIgn = false; //rescuer - distress signal ignored
  distAcc = false; //rescuer - distress signal accepted

  // Initial state is rescuer menu
  situation = 3;

  // Add click handlers
  button1.attachClick(handleButton1Click);
  button1.attachDoubleClick(doNothing);
  // For testing purposes:
  button1.attachLongPressStart(handleButton2Click);
  button1.setPressTicks(300); //time to distinguish click vs long press
  button1.setClickTicks(500); //time to distinguish click vs double click
  
  button2.attachClick(handleButton2Click);
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

void Rescuer_loop() {
  //getupdated gps

  gpsVal = gps.location.isValid();
  if (gpsVal) {
    currLat = gps.location.lat();
    currLong = gps.location.lng();
  }

  unsigned long currentRescuerTime = millis();
  //OLED input strings
  String str[8] = {"","","","","","","",""};
  situation = getSituation();
  
  switch(situation){
    case 0:
    case 1:
    case 2:
      break;
    case 3:
      // 3 distress received - those in distress can't be rescuers
      // Check if received distress is a cancellation or not
      if (!distData.cancelFlag) {

        str[0] = "BOAT " + String(distData.address) + " DISTRESS";
  
        // Check if GPS data from the resucee is valid
        if (isValidGps(distData.gpsLat, distData.gpsLong)) {
          str[1] = "LAT: " + String(distData.gpsLat, 5);
          str[2] = "LONG: " + String(distData.gpsLong, 5);
  
          // Calculate distance if both rescuee and rescuer GPS data is valid
          if (gpsVal) {
            float distanceInMeters = gps.distanceBetween(currLat, currLong, distData.gpsLat, distData.gpsLong);
            str[3] = "DIS: " + String(distanceInMeters, 2) + " m";
          }
          // If our GPS is invalid, show not available
          else {
            str[3] = "DIS: Not Available";
          }
        }
        else {
          // Rescuee's GPS data is invalid
          str[1] = "LAT: Not Available";
          str[2] = "LONG: Not Available";
          str[3] = "DIS: Not Available";
        }
        
        str[4] = "AL " + String(distData.alertLevel + 1);
        str[5] = " ";
        str[6] = "BTN 1 RESCUE";
        str[7] = "BTN 2 IGNORE";
  
        showInOled(str);
        //buzzer
        EasyBuzzer.update();        
      } 
      else {
        EasyBuzzer.stopBeep();
        receiveCancelBroadcast();
      }

      // Continuously listen for updates about the distressed vessel
      distRec = mesh.listenForDistressSignal();
      if(distRec) {
        DistressSignal distDataTemp = mesh.getDistressSignal();
        if (distDataTemp.cancelFlag) {
          EasyBuzzer.stopBeep();
          receiveCancelBroadcast();
        }
        else distData = distDataTemp;
      }
      break;
    case 4:
      // 4 distress accepted - to the rescue mode
      //continue to display updates rescuee info with option to cancel

      // What if the received distress signal does not match the original vessel?
      // We should ignore those and just focus on the original vessel
      distRec = mesh.listenForDistressSignal();
      if(distRec){
        //get distress signal data
        DistressSignal distDataTemp = mesh.getDistressSignal();
        if(distDataTemp.address == distData.address){
          // If it's a cancellation, then display cancellation message then go back to default menu.  
          // No response needed.
          if (distDataTemp.cancelFlag) {
            isRescuer = true;
            changeProgramState(CANCEL_DISTRESS);
          }
          else distData = distDataTemp;
        }
      }

      // Display either DETAILS (rescuer details)
      // or NAVAIDS (navigational aids) screen depending
      // on the user's current selection
      if (currentScreen == DETAILS) {
        showRescueeDetails(str, distData, currLat, currLong);
      }
      else if (currentScreen == NAVAIDS) {
        showNavAids(distData, currLat, currLong);
      }

      // continue to get and send rescuer data/distress response
      // Limit to every 30 seconds as to not overwhelm the radio/network
      if (currentRescuerTime - timeLastResponseSent > DISTRESS_RESPONSE_INTERVAL) {
        timeLastResponseSent = currentRescuerTime;
        mesh.sendDistressResponse(distData.address, currLat, currLong);
      }
      break;
    
    case 5:
      //5 distress signal ignored
      //same as default but not listening for distress signal
      //display that you are not listening
      //give option to listen again
      str[0] = "Distress Signals blocked";
      str[1] = "Press button 1 to receive distress signals";
      showInOled(str);
      break;
  } // situation switch
}

/* UIs for Rescuer */
void showRescueeDetails(String str[8], DistressSignal distData, float currLat, float curLong) {
  str[0] = "GOTO BOAT " + String(distData.address);
      
  if (isValidGps(distData.gpsLat, distData.gpsLong)) {
    str[1] = "LAT: " + String(distData.gpsLat, 5);
    str[2] = "LONG: " + String(distData.gpsLong,5 );

    // Calculate distance if both rescuee and rescuer GPS data is valid
    if (gpsVal) {
      float distanceInMeters = gps.distanceBetween(currLat, currLong, distData.gpsLat, distData.gpsLong);
      str[3] = "DIS: " + String(distanceInMeters, 2) + " m";
    }
    else {
      str[3] = "DIS: Not Available";
    }
  } 
  else {
    str[1] = "LAT: Not Available";
    str[2] = "LONG: Not Available";
    str[3] = "DIS: Not Available";
  }
  str[4] = "AL " + String(distData.alertLevel + 1);
  str[5] = " ";
  str[6] = "BTN 2 END/CANCEL";

  showInOled(str);
}

void showNavAids(DistressSignal distData, float currLat, float curLong) {
  oled.clearDisplay();
  oled.setCursor(0, 0);
  
  oled.setTextSize(2);
  oled.println("BOAT 2");

  if (isValidGps(distData.gpsLat, distData.gpsLong) && gpsVal) {
    // Print the current direction
    oled.print("DIREK ");
    String dir = getDirection(currLat, currLong, distData.gpsLat, distData.gpsLong);
    oled.println(dir);
    
    oled.print("DIS ");
    float distanceInMeters = gps.distanceBetween(currLat, currLong, distData.gpsLat, distData.gpsLong);
    oled.print(distanceInMeters, 0);
    oled.println("m");
  }
  else {
    oled.print("DIREK N/A");
    oled.print("DIS N/A");
  }
     
  oled.display();
}

String getDirection(float sourceLat, float sourceLong, float destLat, float destLong) {
  double courseTo = TinyGPSPlus::courseTo(
    sourceLat,
    sourceLong,
    destLat,
    destLong);

  return String(TinyGPSPlus::cardinal(courseTo));
}

// Button handlers*******
void handleButton1Click() {
  //button 1
  int bt = 1;
  Serial.println("Situation " +String(situation)+ " button " + String(bt));
  
  switch(situation){
    case 3:
      //3 distress received - those in distress can't be rescuers
      distAcc = true;
      EasyBuzzer.stopBeep();
      mesh.sendDistressResponse(distData.address, currLat, currLong);
      Serial.println(distAcc);
      Serial.print("Accept? ");
      break;
    case 4:
      //4 distress accepted - to the rescue mode
      // During the rescuing phase, switch screens on a button press
      currentScreen = (currentScreen == DETAILS) ? NAVAIDS : DETAILS;
      break;
    case 5:
      //5 distress signal ignored
      distIgn = false;
      break;
  }
}
void handleButton2Click() {
  //button 2
  int bt = 2;
  switch(situation){
    case 0:
    case 1:
    case 2:
      Serial.println("Situation " +String(situation)+ " button " + String(bt));
      //2 in distress with response
      break;
    case 3:
      Serial.println("Situation " +String(situation)+ " button " + String(bt));
      //3 distress received - those in distress can't be rescuers
      //str[7] = "Press Button 2 to not Rescue";
      distIgn = true;
      distAcc = false;
      timeLastListened = millis();

      cancelRescueMessage();
      EasyBuzzer.stopBeep();
      delay(2000);
      // Change state to rescuer menu
      changeProgramState(DEFAULT_MENU);
      break;
    case 4:
      Serial.println("Situation " +String(situation)+ " button " + String(bt));
      //4 distress accepted - to the rescue mode
      distRec = false;
      distIgn = false;
      distAcc = false;
      cancelRescueMessage();
      delay(2000);
      changeProgramState(DEFAULT_MENU);
      break;
    case 5:
      Serial.println("Situation " +String(situation)+ " button " + String(bt));
      //5 distress signal ignored

  }//situation switch
}

void showInOled(String str[8]){
  oled.clearDisplay();
  oled.setCursor(0, 0);
  //8 lines/rows
  //21 columns/chars per line
  for(int x = 0; x < 8; x++){
    if(str[x].length() > 0){
      if(str[x].length() > 21){
        oled.println("string too long");
      }else{
        oled.println(str[x]);
      }
    }
  }
  oled.display();
  
}//show in OLED

void cancelRescueMessage() {
  oled.clearDisplay(); // clear display
  delay(100); // wait for initializing
  oled.setTextSize(1);          // text size
  oled.setTextColor(WHITE);     // text color
  oled.setCursor(0, 10);        // position to display
  oled.println("Stopping Rescue.");
  oled.println("");
  oled.println("Going back to main menu.");
  oled.display();
}

int getSituation(){
  int sit = 0;
  /*
   * evaluate these:
   *bool inDistress = false; //rescuee
   *bool recRescResp = false; //rescuee - rescue response received
   *bool distRec = false; //rescuer - distress signal received
   *bool distIgn = false; //rescuer - distress signal ignored
   *bool distAcc = false; //rescuer - distress signal accepted
   *  0 default - not in distress and no distress received
   *  1 in distress no response - requesting rescue
   *  2 in distress with response - waiting for rescue
   *  3 distress received - those in distress can't be rescuers
   *  4 distress accepted - to the rescue mode
   *  5 distress signal ignored
   */
  if(distIgn){
    //ignore if they are in trouble themselves - return to rescuee menu and not display data of rescue request received
    sit = 5;
  }
  else{
    //if not yet ingored, continue to wait for response or confirm rescue
    sit = (!distAcc) ? 3 : 4;  
  }
  return sit;
}
