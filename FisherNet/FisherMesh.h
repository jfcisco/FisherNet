/**
 * Reusable library on top of RadioHead that makes
 * it a little easier to work with the mesh network
 * via LoRA.
 */
// Include guards
#ifndef FisherMesh_h
#define FisherMesh_h

// Include all dependencies for the communication via the mesh network
#include <SPI.h>
#include <RH_RF95.h>
#include <RHMesh.h>

// Defines the types of alert levels
typedef enum {
  ALERT_TECHNICAL = 0,
  ALERT_HOSTILE = 1,
  ALERT_SINKING = 2, 
  ALERT_MEDICAL = 3,
  ALERT_GENERAL = 4
}
AlertLevel;

// Possible values for msgType
#define DISTRESS_SIGNAL 10
#define DISTRESS_RESPONSE 20

// We need this to distinguish between distress signals and distress responses
typedef struct {
  uint8_t msgType;
}
DistressHeader;

// Defines the distress signal message
typedef struct {
  DistressHeader header;
  unsigned char address;
  float gpsLat;
  float gpsLong;
  AlertLevel alertLevel;
  unsigned int hopsLeft;
  bool cancelFlag;
}
DistressSignal;

// Defines the distress signal response message
typedef struct {
  DistressHeader header;
  unsigned char address;
  float gpsLat;
  float gpsLong;
}
DistressResponse;

// Set a hop limit to optimize the flooding of the distress signal
// Hop limit dictates how many times the distress signal should be retransmitted
#define DISTRESS_HOP_LIMIT 5

// Defines a helper class for sending and receiving data from the mesh network
class FisherMesh {
public:
  // Constructor. Used by FisherMesh mesh(...);
  FisherMesh(uint8_t address, float frequency);

  // Sets up the mesh network and its dependencies
  // Returns false if an error occurred
  bool init(uint8_t address, float frequency);

  // Triggers a reset in the module
  void resetModule();

  // Sends a distress signal containing the passed in information
  bool sendDistressSignal(float gpsLat, float gpsLong, AlertLevel alertLevel, bool cancelFlag);

  // Sends a distress response message to the given address. This is sent from point-to-point instead of broadcasted.
  bool sendDistressResponse(uint8_t address, float gpsLat, float gpsLong);
  
  // Listens to distress signals  
  bool listenForDistressSignal();
  
  // Listens for distress response signals  
  // Returns true if a distress response signal was received
  bool listenForDistressResponse();

  // Gets the address of this node
  uint8_t getAddress();

  // Gets the frequency of this node's radio
  float getFrequency();

  // Gets the last distress response received (if we are listening for distress responses)
  DistressResponse getResponse();

  // Gets the last distress signal received (if we are listening for distress signals)
  DistressSignal getDistressSignal();

private:
  uint8_t _address;
  uint8_t _buffer[RH_MESH_MAX_MESSAGE_LEN];
  RH_RF95 _rf95;
  RHMesh _manager;
  float _frequency;
  DistressSignal _distressSignal;
  DistressResponse _distressResponse;
  static char* getNameOfAlertLevel(AlertLevel al);
};

// Below is the C++ implementation of the methods above
// No need to look any further unless you want to modify the code
FisherMesh::FisherMesh(uint8_t address, float frequency)
  : _rf95(LORA_CHIP_SELECT, LORA_INTERRUPT),
    _manager(_rf95, address),
    _frequency(frequency),
    _address(address) {

};

bool FisherMesh::init(uint8_t address, float frequency) {
  // Set address fields and frequency
  _address = address;
  _frequency = frequency;
  
  // Ready reset pin
  pinMode(LORA_RESET, OUTPUT);
  digitalWrite(LORA_RESET, HIGH);
  resetModule();

  // Start the mesh manager
  if (!_manager.init()) {
    Serial.println("Mesh Manager failed to init");
    return false;
  }

  // Set the address of this node.
  _manager.setThisAddress(address);

    
  uint8_t meshAdd = _manager.thisAddress();
  Serial.print("Node ");
  Serial.print(meshAdd, DEC);
  Serial.println(" intialized");

  // Set the radio frequency to the correct one
  if (!_rf95.setFrequency(_frequency)) {
    Serial.println("LoRa Module failed to set frequency");
    return false;
  }

  Serial.printf("Frequency set at %f\n", _frequency);
  
  // Set the radio transmit power and modem configuration
#ifdef EGIZMO
  // Ra-02 Module has a maximum transmit power of +18dBm per datasheet
  _rf95.setTxPower(18);
#endif

#ifdef LILYGO
  // T-Beam has a maximum transmit power of +20dBm per datasheet
  _rf95.setTxPower(20);
#endif

  // Use one of the slow+long range configurations
  // Reference https://www.airspayce.com/mikem/arduino/RadioHead/classRH__RF95.html#ab9605810c11c025758ea91b2813666e3
  const RH_RF95::ModemConfigChoice modemConfig = RH_RF95::ModemConfigChoice::Bw125Cr48Sf4096;
  
  if (!_rf95.setModemConfig(modemConfig)) {
    Serial.println("LoRa modem configuration failed");
    return false;
  }
  Serial.println("LoRa model configuration succeeded");

  // Print out modem registers to serial to verify if the modem config was successful
  Serial.println("Configured registers for LoRa module:");
  _rf95.printRegisters();
  
  return true;
}

void FisherMesh::resetModule() {
  digitalWrite(LORA_RESET, LOW);
  delay(10);
  digitalWrite(LORA_RESET, HIGH);
  delay(10);
};

// Sends a distress signal containing the passed in information
bool FisherMesh::sendDistressSignal(float gpsLat, float gpsLong, AlertLevel alertLevel, bool cancelFlag) {
  // Setup the distress signal struct
  _distressSignal.header.msgType = DISTRESS_SIGNAL;
  _distressSignal.address = _address;
  _distressSignal.gpsLat = gpsLat;
  _distressSignal.gpsLong = gpsLong;
  _distressSignal.alertLevel = alertLevel;
  _distressSignal.hopsLeft = DISTRESS_HOP_LIMIT;
  _distressSignal.cancelFlag = cancelFlag;
  
  // Copy the distress signal into a byte buffer
  // This is needed to send the data over radio
  memcpy(_buffer, &_distressSignal, sizeof(_distressSignal));
  
  // Try to broadcast the distress signal
  if (_manager.sendtoWait(
      // Pass in the distressSignal as an array of bytes
      (uint8_t *)_buffer,
      sizeof(_distressSignal),
      // Broadcast the signal instead of directing the signal to a specific node
      RH_BROADCAST_ADDRESS) == RH_ROUTER_ERROR_NONE) {
        
        // The signal has been reliably delivered to a node.
        return true; 
      }
  else
    return false;
};

// Sends a distress response message to the given address. This is sent from point-to-point instead of broadcasted.
bool FisherMesh::sendDistressResponse(uint8_t address, float gpsLat, float gpsLong) {
  // Build a distressResponse struct
  _distressResponse.header.msgType = DISTRESS_RESPONSE;
  _distressResponse.address = _address;
  _distressResponse.gpsLat = gpsLat;
  _distressResponse.gpsLong = gpsLong;
  
  // Copy distressResponse to buffer as an array of bytes
  memcpy(_buffer, &_distressResponse, sizeof(_distressResponse));
  
  // Send data in buffer back to the address
  return _manager.sendtoWait(_buffer, sizeof(_distressResponse), address) == RH_ROUTER_ERROR_NONE;
};
    
// Listens for distress signals  
bool FisherMesh::listenForDistressSignal() {
  uint8_t len = sizeof(_buffer);
  uint8_t from;
  
  if (_manager.recvfromAck(_buffer, &len, &from)) {
    // We can do this because both types of signals 
    // (i.e. DistressResponse and DistressSignal) have a DistressHeader.
    DistressHeader *header = (DistressHeader *)_buffer;
    
    if (len > 1 && header->msgType == DISTRESS_SIGNAL) {
#ifdef DEBUG_MODE
      Serial.print("Got a distress signal from: ");
      Serial.println(from, DEC);
#endif
      // Cast the response into the struct
      DistressSignal *distressSignal = (DistressSignal *)header;
      _distressSignal = *distressSignal;

      if (distressSignal->hopsLeft > 0) {
        // Decrement the distress signal's hops
        distressSignal->hopsLeft--;
#ifdef DEBUG_MODE
        Serial.print("Rebroadcasting distressSignal with hopsLeft= ");
        Serial.println(distressSignal->hopsLeft, DEC);
#endif
        // Copy the distress signal to buffer as an array of bytes
        memcpy(_buffer, distressSignal, sizeof(DistressSignal));
        
        // Rebroadcast the signal while mimicking the source
        _manager.sendtoWait(  
          (uint8_t *)_buffer,
          sizeof(DistressSignal),
          RH_BROADCAST_ADDRESS);
      } 
      return true;
    }
  }
  return false;
};
    
// Listens for distress response signals  
// Returns true if a distress response signal was received
bool FisherMesh::listenForDistressResponse() {
  uint8_t len = sizeof(_buffer);
  uint8_t from;
  
  if (_manager.recvfromAck(_buffer, &len, &from)) {
    // We can do this because both types of signals 
    // (i.e. DistressResponse and DistressSignal) have a DistressHeader.
    DistressHeader *header = (DistressHeader *)_buffer;
    
    if (len > 1 && header->msgType == DISTRESS_RESPONSE) {
      // Cast the response into the struct
      DistressResponse *response = (DistressResponse *)header;
      _distressResponse = *response;
      return true;
    }
    // Case when a distressed vessel catches a distress signal from another distress vessel
    else if (len > 1 && header->msgType == DISTRESS_SIGNAL) {
#ifdef DEBUG_MODE
      Serial.print("Got a distress signal from: ");
      Serial.println(from, DEC);
#endif
      // Cast the response into the struct
      DistressSignal *distressSignal = (DistressSignal *)header;

      if (distressSignal->hopsLeft > 0) {
        // Decrement the distress signal's hops
        distressSignal->hopsLeft--;
#ifdef DEBUG_MODE
        Serial.print("Rebroadcasting distressSignal with hopsLeft= ");
        Serial.println(distressSignal->hopsLeft, DEC);
#endif
        // Copy the distress signal to buffer as an array of bytes
        memcpy(_buffer, distressSignal, sizeof(DistressSignal));
        
        // Rebroadcast the signal while mimicking the source
        _manager.sendtoWait(  
          (uint8_t *)_buffer,
          sizeof(DistressSignal),
          RH_BROADCAST_ADDRESS);
      }
    }
  }
  return false;
};

uint8_t FisherMesh::getAddress() {
  return _manager.thisAddress();
}

float FisherMesh::getFrequency() {
  return _frequency;
}

DistressResponse FisherMesh::getResponse() {
  return _distressResponse;
};

DistressSignal FisherMesh::getDistressSignal() {
  return _distressSignal;
}

#endif // FisherMesh_h
