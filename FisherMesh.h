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

// Defines the types of alert level
typedef enum {
  ALERT_TECHNICAL = 0,
  ALERT_SECURITY = 1,
  ALERT_SAFETY = 2,
  ALERT_HOSTILE = 3,
  ALERT_GENERAL = 4
}
AlertLevel;

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

// Defines a helper class for sending and receiving data from the mesh network
class FisherMesh {
public:
  // Constructor. Used by FisherMesh mesh(...);
  FisherMesh(uint8_t address, float frequency);

  // Sets up the mesh network and its dependencies
  // Returns false if an error occurred
  bool init();

  // Triggers a reset in the module
  void resetModule();

  // Sends a distress signal containing the passed in information
  bool sendDistressSignal(float gpsLat, float gpsLong, AlertLevel alertLevel);

  // Sends a distress response message to the given address. This is sent from point-to-point instead of broadcasted.
  bool sendDistressResponse(uint8_t address, float gpsLat, float gpsLong);
  
  // Listens to distress signals  
  bool listenForDistressSignal();
  
  // Listens for distress response signals  
  // Returns true if a distress response signal was received
  bool listenForDistressResponse();

  // Gets the address of this node
  uint8_t getAddress();

  // Gets the last distress response received (if we are listening for distress responses)
  DistressResponse getResponse();

  // Gets the last distress signal received (if we are listening for distress signals)
  DistressSignal getDistessSignal();

  static const char* getNameOfAlertLevel()
  {
    
  }

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

// C++ implementation of the methods above
// No need to look any further unless you want to modify the code
FisherMesh::FisherMesh(uint8_t address, float frequency = 433.0)
  : _rf95(LORA_CHIP_SELECT, LORA_INTERRUPT),
    _manager(_rf95, address),
    _frequency(frequency),
    _address(address) {

};

bool FisherMesh::init() {
  // Ready reset pin
  pinMode(LORA_RESET, OUTPUT);
  digitalWrite(LORA_RESET, HIGH);
  resetModule();

  // Start the mesh manager
  if (!_manager.init()) {
    Serial.println("Mesh Manager failed to init");
    return false;
  }

  // Set the radio frequency to the correct one
  if (!_rf95.setFrequency(_frequency)) {
    Serial.println("LORA Module failed to set frequency");
    return false;
  }

  return true;
}

void FisherMesh::resetModule() {
  digitalWrite(LORA_RESET, LOW);
  delay(10);
  digitalWrite(LORA_RESET, HIGH);
  delay(10);
};

// Sends a distress signal containing the passed in information
bool FisherMesh::sendDistressSignal(float gpsLat, float gpsLong, AlertLevel alertLevel) {
  // Setup the distress signal struct
  _distressSignal.header.msgType = DISTRESS_SIGNAL;
  _distressSignal.address = _address;
  _distressSignal.gpsLat = gpsLat;
  _distressSignal.gpsLong = gpsLong;
  _distressSignal.alertLevel = alertLevel;
  
  // Copy the distress signal into a byte buffer
  // This is needed to send the data over radio
  memcpy(_buffer, &_distressSignal, sizeof(_distressSignal));
  
  // Try to broadcast the distress signal
  if (_manager.sendtoWait(
      // Pass in the distressSignal as an array of bytes
      (uint8_t *)_buffer,
      sizeof(_distressSignal),
      // Broadcast the signal instead of directing the signal to the 
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
  return _manager.sendtoWait(_buffer, sizeof(_distressResponse), address) != RH_ROUTER_ERROR_NONE;
};
    
// Listens to distress signals  
bool FisherMesh::listenForDistressSignal() {
  uint8_t len = sizeof(_buffer);
  uint8_t from;
  
  if (_manager.recvfromAck(_buffer, &len, &from))
  {
    // We can do this because both types of signals 
    // (i.e. DistressResponse and DistressSignal) have a DistressHeader.
    DistressHeader *header = (DistressHeader *)_buffer;
    
    if (len > 1 && header->msgType == DISTRESS_SIGNAL) {
      // Cast the response into the struct
      DistressSignal *distressSignal = (DistressSignal *)header;
      _distressSignal = *distressSignal;
    }

    return true;
  }
  return false;
};
    
// Listens for distress response signals  
// Returns true if a distress response signal was received
bool FisherMesh::listenForDistressResponse() {
  uint8_t len = sizeof(_buffer);
  uint8_t from;
  
  if (_manager.recvfromAck(_buffer, &len, &from))
  {
    // We can do this because both types of signals 
    // (i.e. DistressResponse and DistressSignal) have a DistressHeader.
    DistressHeader *header = (DistressHeader *)_buffer;
    
    if (len > 1 && header->msgType == DISTRESS_RESPONSE) {
      // Cast the response into the struct
      DistressResponse *response = (DistressResponse *)header;
      _distressResponse = *response;
    }

    return true;
  }
  return false;
};

uint8_t FisherMesh::getAddress() {
  return _address;
}

DistressResponse FisherMesh::getResponse() {
  return _distressResponse;
};

DistressSignal FisherMesh::getDistessSignal() {
  return _distressSignal;
}

#endif // FisherMesh_h
