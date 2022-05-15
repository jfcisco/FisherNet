/**
.h * Preferences: 
 * - Stores settings such as node address and LoRa frequency
 * to make it easier to setup the devices. 
 * 
 * - There won't be a need to change the hard coded values
 * in the code before uploading.
 * 
 * - Warning: Pin assignments are still hard-coded
 */
// Gets the part where the user specifies GET/SET
// examples of user arg are GET ADD, SET ADD <new-address>
String getCommand(String userArg) {
  return userArg.substring(0, 3);
}

String getOption(String userArg) {
  return userArg.substring(4, 7);
}

// Reads the unsigned 8-bit integer provided by the user
// Used for reading values provided in SET <X> arguments
// Assumes values are from index 7 onwards
uint8_t readUInt8FromArgs(String userArg) {
  long maybeValidUInt = userArg.substring(7).toInt();

  // 8-bit integers are only from [0, 255]
  return constrain(maybeValidUInt, 0, 255);
}

float readFloatFromArgs(String userArg) {
  return userArg.substring(7).toFloat();
}

void parseArgs(String userArg) {
  userArg.trim();
  userArg.toUpperCase();
  
  if (getCommand(userArg) == "GET") {
      String option = getOption(userArg);

      if (option == "ADD") {
        uint8_t address = getAddress();
        Serial.print("Node Address: ");
        Serial.println(address, DEC);
      }
      else if (option == "FRQ") {
        float freq = getFrequency();
        Serial.print("LoRa Frequency: ");
        Serial.println(freq, 1);
      }
      else printError();
  }
  else if (getCommand(userArg) == "SET") {
    String option = getOption(userArg);

    if (option == "ADD") {
      uint8_t provided = readUInt8FromArgs(userArg);
      bool successfulSet = setAddress(provided);
      if (!successfulSet) {
        Serial.println("Error occured: while setting new address");
        return;
      }

      Serial.print("Successfully set address to ");
      Serial.println(provided, DEC);
    }
    else if (option == "FRQ") {
      float provided = readFloatFromArgs(userArg);
      bool successfulSet = setFrequency(provided);
      if (!successfulSet) {
        Serial.println("Error occured: while setting new frequency");
        return;
      }

      Serial.print("Successfully set frequency to ");
      Serial.print(provided, 1);
      Serial.println(" MHz");
    }
    else printError();
  }
  else printError();
}

// Error message when invalid input was provided (for example, address outside 0-255)
void printError() {
  Serial.println("You provided invalid input.");
  Serial.println("Please double check.");
}

uint8_t getAddress() {
  bool prefSuccess = preferences.begin("my-app", false);
  if (!prefSuccess) {
    Serial.println("Error: Opening preferences failed. Returning default value...");
    preferences.end();
    return 0;
  }
  
  uint8_t address = preferences.getUChar("address", 255);
  preferences.end();
  return address;
}

bool setAddress(uint8_t newAddress) {
  bool prefSuccess = preferences.begin("my-app", false);
  if (!prefSuccess) {
    Serial.println("Error: Opening preferences failed.");
    preferences.end();
    return false;
  }
  
  prefSuccess = preferences.putUChar("address", newAddress);
  preferences.end();
  
  if (!prefSuccess) {
    Serial.println("Error: Could not store address");
    preferences.end();
    return false;
  }
  return true;
}

float getFrequency() {
  bool prefSuccess = preferences.begin("my-app", false);
  if (!prefSuccess) {
    Serial.println("Error: Opening preferences failed. Returning default value...");
    preferences.end();
    return 433.0; // 923.0 MHz for SG
  }
  
  float frequency = preferences.getFloat("frequency", 433.0); // 923.0 MHz for SG
  preferences.end();

  return frequency;
}

bool setFrequency(float newFreq) {
  bool prefSuccess = preferences.begin("my-app", false);
  if (!prefSuccess) {
    Serial.println("Error: Opening preferences failed.");
    preferences.end();
    return false;
  }

  // Constrain to a valid frequency range
  newFreq = constrain(newFreq, 400.0, 1000.0);
  prefSuccess = preferences.putFloat("frequency", newFreq);
  preferences.end();
  
  if (!prefSuccess) {
    Serial.println("Error: Could not store new frequency");
    preferences.end();
    return false;
  }
  return true;
}
