float currLat = 999.0;
float currLong = 999.0;
bool gpsVal = false;
//Distress signal global vars
DistressSignal distData;
unsigned long timeLastResponseSent;

//global situaion variables
bool distIgn; //rescuer - distress signal ignored
bool distAcc; //rescuer - distress signal accepted
int situation;
// Situation could have been an enum
/*
Something like:
typedef enum {
  DEFAULT_STATE,
  DISTRESS_NO_RESPONSE,
  DISTRESS_WITH_RESPONSE,
  DISTRESS_RECEIVED,
  DISTRESS_ACCEPTED,
  DISTRESS_IGNORED
}
Situation;
*/

/*
 *  0 default - not in distress and no distress received
 *  1 in distress no response
 *  2 in distress with response
 *  3 distress received - those in distress can't be rescuers
 *  4 distress accepted - to the rescue mode
 *  5 distress signal ignored
 */

 void Rescuer_setup(DistressSignal distress) {
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  
  // Initialize variables
  distData = distress;
  // distData = dummyDistress;
  timeLastResponseSent = 0;
  distRec = true; //rescuer - distress signal received
  distIgn = false; //rescuer - distress signal ignored
  distAcc = false; //rescuer - distress signal accepted
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
  button3.attachClick(handleButton3Click);
  button3.attachDoubleClick(doNothing);
  button3.attachLongPressStart(doNothing);
  button4.attachClick(handleButton4Click);
  button4.attachDoubleClick(doNothing);
  button4.attachLongPressStart(doNothing);
  button5.attachClick(handleButton5Click);
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
#ifdef DEBUG_MODE
  Serial.println("Situation: " + String(situation));
#endif
  
  switch(situation){
    case 0:
    case 1:
    case 2:
      break;
    case 3:
      //3 distress received - those in distress can't be rescuers
      //display rescuee info
      str[0] = "BOAT " + String(distData.address) + " DISTRESS";

      // Check if GPS data from the resucee is valid
      if (isValidGps(distData.gpsLat, distData.gpsLong)) {
        str[1] = "LAT: " + String(distData.gpsLat);
        str[2] = "LONG: " + String(distData.gpsLong);

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
      break;
    case 4:
      //4 distress accepted - to the rescue mode
      //continue to display updates rescuee info with option to cancel

      // What if the received distress signal does not match the original vessel?
      // We should ignore those and just focus on the original vessel
      distRec = mesh.listenForDistressSignal();
      if(distRec){
        //get distress signal data
        DistressSignal distDataTemp = mesh.getDistressSignal();
        if(distDataTemp.address == distData.address){
          distData = distDataTemp;
        }
      }
      
      str[0] = "GOTO BOAT " + String(distData.address);
      str[1] = "LAT: " + String(distData.gpsLat);
      str[2] = "LONG: " + String(distData.gpsLong);

      if (isValidGps(distData.gpsLat, distData.gpsLong)) {
        str[1] = "LAT: " + String(distData.gpsLat);
        str[2] = "LONG: " + String(distData.gpsLong);

        // Calculate distance if both rescuee and rescuer GPS data is valid
        if (gpsVal) {
          float distanceInMeters = gps.distanceBetween(currLat, currLong, distData.gpsLat, distData.gpsLong);
          str[3] = "DIS: " + String(distanceInMeters, 2);
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

      // continue to get and send rescuer data/distress response
      // Limit to every 30 seconds as to not overwhelm the radio/network
      if (currentRescuerTime - timeLastResponseSent > 30000) {
        timeLastResponseSent = currentRescuerTime;
        distAcc = mesh.sendDistressResponse(distData.address, currLat, currLong);
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

#ifdef DEBUG_MODE
  Serial.print("distAcc: ");
  Serial.println(distAcc);
#endif
}

// Button handlers*******
void handleButton1Click() {
  //button 1
  int bt = 1;
  switch(situation){
    case 0:
    case 1:
    case 2:
      Serial.println("Situation " +String(situation)+ " button " + String(bt));
      break;
    case 3:
      Serial.println("Situation " +String(situation)+ " button " + String(bt));
      //3 distress received - those in distress can't be rescuers
      distAcc = mesh.sendDistressResponse(distData.address, currLat, currLong);
      Serial.print("Accept? ");
      Serial.println(distAcc);
      break;
    case 4:
      Serial.println("Situation " +String(situation)+ " button " + String(bt));
      //4 distress accepted - to the rescue mode
//      distRec = false;
//      distIgn = false;
//      distAcc = false;
//      break;
    case 5:
      Serial.println("Situation " +String(situation)+ " button " + String(bt));
      //5 distress signal ignored
      distIgn = false;
  }//situation switch
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

      cancelRescueMessage();
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
void handleButton3Click() {
  //button 3
  int bt = 3;
  switch(situation){
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
      Serial.println("Situation " +String(situation)+ " button " + String(bt));
      break;
  }//situation switch
}
void handleButton4Click() {
  //button 4
  int bt = 4;
  switch(situation){
    case 0:
      Serial.println("Situation " +String(situation)+ " button " + String(bt));
      //default - not in distress and no distress received
      break;
    case 1:
      Serial.println("Situation " +String(situation)+ " button " + String(bt));
      //1 in distress no response
      break;
    case 2:
      Serial.println("Situation " +String(situation)+ " button " + String(bt));
      //2 in distress with response
      break;
    case 3:
      Serial.println("Situation " +String(situation)+ " button " + String(bt));
      //3 distress received - those in distress can't be rescuers
      break;
    case 4:
      Serial.println("Situation " +String(situation)+ " button " + String(bt));
      //4 distress accepted - to the rescue mode
      break;
    case 5:
      Serial.println("Situation " +String(situation)+ " button " + String(bt));
      //5 distress signal ignored
  }//situation switch
}
void handleButton5Click() {
  //button 5
  int bt = 5;
  switch(situation){
    case 0:
      Serial.println("Situation " +String(situation)+ " button " + String(bt));
      //default - not in distress and no distress received
      break;
    case 1:
      Serial.println("Situation " +String(situation)+ " button " + String(bt));
      //1 in distress no response
      break;
    case 2:
      Serial.println("Situation " +String(situation)+ " button " + String(bt));
      //2 in distress with response
      break;
    case 3:
      Serial.println("Situation " +String(situation)+ " button " + String(bt));
      //3 distress received - those in distress can't be rescuers
      break;
    case 4:
      Serial.println("Situation " +String(situation)+ " button " + String(bt));
      //4 distress accepted - to the rescue mode
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
  //Serial.println("Start of sit eval = in distress " + String(inDistress) +"+ distress rec " +String(distRec)+ " = Situation: " + String(sit));
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
