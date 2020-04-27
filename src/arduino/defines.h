#include <ArduinoBLE.h>
#include <Adafruit_NeoPixel.h>

#define MAX_LEDS_PER_CHANNEL 300
#define MAX_CHANNELS 8
#define NUM_ROWS 4
#define NUM_COLS 3
#define UUID_LENGTH 37

#define SIGNAL_DELAY 500

const int interiorTogglePin = 10;
const int alternateCommandTogglePin = 11;
const int leftTurnTogglePin = A0;
const int rightTurnTogglePin = A1;
const int brakeTogglePin = A2;
const int outputPins[MAX_CHANNELS] = {2, 3, 4, 5, 6, 7, 8, 9};

const char serviceUUID[UUID_LENGTH] = {"8d727090-093e-46a8-bf1d-23eeee2da202"};

const char characteristicUUIDs[12][UUID_LENGTH] = {
  "8d727091-093e-46a8-bf1d-23eeee2da202", //save
  "8d727092-093e-46a8-bf1d-23eeee2da202", //on
  "8d727093-093e-46a8-bf1d-23eeee2da202", //command
  "8d727094-093e-46a8-bf1d-23eeee2da202", //bulkRead
  "8d727095-093e-46a8-bf1d-23eeee2da202", //channel 1
  "8d727096-093e-46a8-bf1d-23eeee2da202", //channel 2
  "8d727097-093e-46a8-bf1d-23eeee2da202", //channel 3
  "8d727098-093e-46a8-bf1d-23eeee2da202", //channel 4
  "8d727099-093e-46a8-bf1d-23eeee2da202", //channel 5
  "8d72709a-093e-46a8-bf1d-23eeee2da202", //channel 6
  "8d72709b-093e-46a8-bf1d-23eeee2da202", //channel 7
  "8d72709c-093e-46a8-bf1d-23eeee2da202" //channel 8
};

typedef struct channelState{
  boolean isCentered; //8 bit 2
  boolean isInterior; //8 bit 2
  boolean stripUsed; //8 bit 2
  byte stripType; //8 bit 2
  byte stripOrder; // 8 bit 2
  byte stripPosition; //8 bit 2
  word numLEDs; //16 bit 4
  word height; //16 bit 4
  word width; //16 bit 4
};

union channelUnion{
  channelState packet;
  byte serializedArray[sizeof(channelState)];
};

typedef struct commandState{
  byte primaryRed;
  byte primaryGreen;
  byte primaryBlue;
  byte secondaryRed;
  byte secondaryGreen;
  byte secondaryBlue;
  byte animation;
  word stepDelay;
};

union commandUnion{
  commandState packet;
  byte serializedArray[sizeof(commandState)];
};

typedef struct bulkRead{
  bool on;
  commandState command;
  channelState channels[MAX_CHANNELS];
};

union bulkReadUnion{
  bulkRead packet;
  byte serializedArray[sizeof(bulkRead)];
};

typedef struct dynamicState{
   channelState channels[MAX_CHANNELS];
   commandState command;
   byte mappedPositions[4][3][MAX_CHANNELS];
   int validationKey;
   bool hasUpdated;
};

typedef struct staticState{
  BLEService *data;
  BLEBoolCharacteristic *on;
  BLEBoolCharacteristic *save;
  BLECharacteristic *command;
  BLECharacteristic *channels[MAX_CHANNELS];
  BLECharacteristic *bulkRead;
  Adafruit_NeoPixel *strips[MAX_CHANNELS];
};

typedef struct tempState{
  bool leftTurnLight;
  bool rightTurnLight;
  bool brakeLight;
  bool interiorOn;
  bool alternateCommand;
  bool lightsOn;
  word stepIndex;
  word lastPosUsed;
  int lastRowUsed;
  bool lightsSaved;
  uint32_t savedLightState[MAX_CHANNELS][MAX_LEDS_PER_CHANNEL];
};

typedef struct state{
  dynamicState dynamic;
  staticState constant;
  tempState temp;
};
