float currLat = 999.0;
float currLong = 999.0;
bool gpsVal = false;
//Distress signal global vars
DistressSignal distData;

//global situaion variables
bool inDistress; //rescuee - rescue requested
bool recRescResp; //rescuee - rescue response received
bool distRec; //rescuer - distress signal received
bool distIgn; //rescuer - distress signal ignored
bool distAcc; //rescuer - distress signal accepted
int situation;
/*
 *  0 default - not in distress and no distress received
 *  1 in distress no response
 *  2 in distress with response
 *  3 distress received - those in distress can't be rescuers
 *  4 distress accepted - to the rescue mode
 *  5 distress signal ignored
 */

 void Rescuer_setup() {
  // Initialize variables
  inDistress = false; //rescuee - rescue requested
  recRescResp = false; //rescuee - rescue response received
  distRec = false; //rescuer - distress signal received
  distIgn = false; //rescuer - distress signal ignored
  distAcc = false; //rescuer - distress signal accepted
  situation = 0;

  // Add click handlers
  button1.attachClick(handleButton1Click);
  button2.attachClick(handleButton2Click);
  button3.attachClick(handleButton3Click);
  button4.attachClick(handleButton4Click);
  button5.attachClick(handleButton5Click);
 }

void Rescuer_loop() {
  //getupdated gps

  gpsVal = gps.location.isValid();
  if (gpsVal) {
    currLat = gps.location.lat();
    currLong = gps.location.lng();
  }

  //OLED input strings
  String str[8] = {"","","","","","","",""};
  situation = getSituation();
  Serial.println("Situation: " + String(situation));
  
  // Jasper: It's best to put the code in each case in separate functions
  switch(situation){
    case 0:
      //default - not in distress and no distress received

      //sample strings
      str[0] = "Situation Zero";
      str[1] = "Not in distress";
      str[2] = "No distress received";
      str[3] = "Press a button";
      showInOled(str);
      //listen for distress signal
      distRec = mesh.listenForDistressSignal();
      if(distRec){
        //get distress signal data
        distData = mesh.getDistessSignal();
      }
      //give option to be the one to send the distress      
      break;
    case 1:
      //1 in distress no response
      break;
    case 2:
      //2 in distress with response
      break;
    case 3:
      //3 distress received - those in distress can't be rescuers
      //display rescuee info
      
      str[0] = "Boat ID: " + String(distData.address) + " in DISTRESS!";
      str[1] = "Latitude: " + String(distData.gpsLat);
      str[2] = "Longitude: " + String(distData.gpsLong);

      // Check if GPS data is valid
      if(gpsVal){
        str[3] = "Distance: " + String(gps.distanceBetween(currLat, currLong, distData.gpsLat, distData.gpsLong));
      }else{
        str[3] = "Distance: GPS data not available";
      }
      str[4] = "Alert Level: " + String(distData.alertLevel);
      str[5] = " ";
      str[6] = "Press Button 1 to Rescue";
      str[7] = "Press Button 2 to not Rescue";

      showInOled(str);
     
      //give response options
      
      break;
    case 4:
      //4 distress accepted - to the rescue mode
      //continue to display updates rescuee info with option to cancel
      str[0] = "Boat ID: " + String(distData.address) + " in DISTRESS!";
      str[1] = "Latitude: " + String(distData.gpsLat);
      str[2] = "Longitude: " + String(distData.gpsLong);


      if(gpsVal){
        str[3] = "Distance: " + String(gps.distanceBetween(currLat, currLong, distData.gpsLat, distData.gpsLong));
      }else{
        str[3] = "Distance: GPS data not available";
      }
      str[4] = "Alert Level: " + String(distData.alertLevel);
      str[5] = " ";
      str[6] = "Press Button 1 to End/Cancel Rescue";

      showInOled(str);      
      // continue to get and send rescuer data/distress response
      distAcc = mesh.sendDistressResponse(distData.address, currLat, currLong);
      
      break;
    case 5:
      //5 distress signal ignored
      //same as default but not listening for distress signal
      //display that you are not listening
      //give option to listen again
      str[0] = "Distress Signals blocked";
      str[1] = "Press button 1 to receive distress signals";
      showInOled(str);

  } // situation switch
}

// Button handlers*******
void handleButton1Click() {
  //button 1
  int bt = 1;
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
      //str[6] = "Press Button 1 to Rescue";
      distAcc = mesh.sendDistressResponse(distData.address, currLat, currLong);
      break;
    case 4:
      Serial.println("Situation " +String(situation)+ " button " + String(bt));
      //4 distress accepted - to the rescue mode
      //"Press Button 1 to End/Cancel Rescue"; - everything goes back to normal
      distRec = false;
      distIgn = false;
      distAcc = false;
      break;
    case 5:
      Serial.println("Situation " +String(situation)+ " button " + String(bt));
      //5 distress signal ignored
      //str[1] = "Press button 1 to receive distress signals";
      distIgn = false;
        
  }//situation switch
}
void handleButton2Click() {
  //button 2
  int bt = 2;
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
      //str[7] = "Press Button 2 to not Rescue";
      distIgn = true;
      distAcc = false;

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
void handleButton3Click() {
  //button 3
  int bt = 3;
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
  //8 lines/rows
  //21 columns/chars per line
  for(int x = 0; x < sizeof(str); x++){
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
  if(!inDistress && !distRec){
    sit = 0;
  }else{
    if(inDistress){
      //rescuee possibilities after rescue request sent
      sit = (!recRescResp) ?  1 : 2;
    }else{
      //not in distress but distress received - rescue or not
      //distRec should be true at this point
      //if not yet accepted / waiting for response situation 3
      if(distIgn){
        //ignore if they are in trouble themselves - return to rescuee menu and not display data of rescue request received
        sit = 5;
      }else{
        //if not yet ingored, continue to wait for response or confirm rescue
        sit = (!distAcc) ? 3 : 4;  
      }
    }//if-else inDistress end
  }//if-else (!inDistress && !distRec)

  //Serial.println("end of start eval = in distress " + String(inDistress) +"+ distress rec " +String(distRec)+ " = Situation: " + String(sit));
  return sit;
}// end of evalSituation()
