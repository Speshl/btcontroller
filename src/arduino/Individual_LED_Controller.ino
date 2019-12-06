//FASTLED doesn't appear to be supported yet on the arduino nano 33 IOT
#include <FlashStorage.h>
#include <ArduinoBLE.h>
#include <Adafruit_NeoPixel.h>

#define MAX_LEDS_PER_CHANNEL 300
#define MAX_COMMANDS 1
#define MAX_CHANNELS 8
#define UUID_LENGTH 37

const int outputPins[MAX_CHANNELS] = {3,4,5,6,7,8,9};

const char generalUUIDs[3][UUID_LENGTH] = {
  "8d727099-093e-46a8-bf1d-23eeee2da202",
  "8d72709a-093e-46a8-bf1d-23eeee2da202",
  "8d72709b-093e-46a8-bf1d-23eeee2da202"  
};

const char channelUUIDs[MAX_CHANNELS][UUID_LENGTH] = {
  "601e09c0-80c6-48b6-af07-8b3e972db14b",
  "00ffb030-9cc8-4f4e-80d8-a5d0391e5060",
  "9cac5610-3cce-4b5b-8f09-7befa63be3c9",
  "12d2c3e0-1c17-4ae8-9307-5f400dc5c6e4",
  "20a734d0-1e9a-4da4-bc33-b914db19b956",
  "9452f330-e8ad-4db0-ab2b-779c57765093",
  "aa2b7ad0-7721-4d79-895f-298a8addb129",
  "bca82d00-f7f4-45c9-a261-ef4b55584f5b"
};

const char channelNumLEDsUUIDs[MAX_CHANNELS][UUID_LENGTH] = {
  "601e09c1-80c6-48b6-af07-8b3e972db14b",
  "00ffb031-9cc8-4f4e-80d8-a5d0391e5060",
  "9cac5611-3cce-4b5b-8f09-7befa63be3c9",
  "12d2c3e1-1c17-4ae8-9307-5f400dc5c6e4",
  "20a734d1-1e9a-4da4-bc33-b914db19b956",
  "9452f331-e8ad-4db0-ab2b-779c57765093",
  "aa2b7ad1-7721-4d79-895f-298a8addb129",
  "bca82d01-f7f4-45c9-a261-ef4b55584f5b"
};

const char channelStripTypeUUIDs[MAX_CHANNELS][UUID_LENGTH] = {
  "601e09c2-80c6-48b6-af07-8b3e972db14b",
  "00ffb032-9cc8-4f4e-80d8-a5d0391e5060",
  "9cac5612-3cce-4b5b-8f09-7befa63be3c9",
  "12d2c3e2-1c17-4ae8-9307-5f400dc5c6e4",
  "20a734d2-1e9a-4da4-bc33-b914db19b956",
  "9452f332-e8ad-4db0-ab2b-779c57765093",
  "aa2b7ad2-7721-4d79-895f-298a8addb129",
  "bca82d02-f7f4-45c9-a261-ef4b55584f5b"
};

const char channelPositionUUIDs[MAX_CHANNELS][UUID_LENGTH] = {
  "601e09c3-80c6-48b6-af07-8b3e972db14b",
  "00ffb033-9cc8-4f4e-80d8-a5d0391e5060",
  "9cac5613-3cce-4b5b-8f09-7befa63be3c9",
  "12d2c3e3-1c17-4ae8-9307-5f400dc5c6e4",
  "20a734d3-1e9a-4da4-bc33-b914db19b956",
  "9452f333-e8ad-4db0-ab2b-779c57765093",
  "aa2b7ad3-7721-4d79-895f-298a8addb129",
  "bca82d03-f7f4-45c9-a261-ef4b55584f5b"
};

const char commandUUIDs[MAX_COMMANDS][UUID_LENGTH] = {
  "f1ca8500-6f92-4c90-adcf-86932eae494e"
};

const char commandPrimaryColorUUIDs[MAX_COMMANDS][UUID_LENGTH] = {
  "f1ca8501-6f92-4c90-adcf-86932eae494e"
};

const char commandSecondaryColorUUIDs[MAX_COMMANDS][UUID_LENGTH] = {
  "f1ca8502-6f92-4c90-adcf-86932eae494e"
};

const char commandDelayUUIDs[MAX_COMMANDS][UUID_LENGTH] = {
  "f1ca8504-6f92-4c90-adcf-86932eae494e"
};

const char commandAnimationUUIDs[MAX_COMMANDS][UUID_LENGTH] = {
  "f1ca8505-6f92-4c90-adcf-86932eae494e"
};
const char commandChannelToggleUUIDs[MAX_COMMANDS][UUID_LENGTH] = {
  "f1ca8507-6f92-4c90-adcf-86932eae494e"
};

union ArrayToInteger {
  byte array[4];
  int integer;
};

struct generalServiceStruct {
  BLEService *general;
  BLEBoolCharacteristic *on;
  BLEBoolCharacteristic *save;
};
typedef struct generalServiceStruct GeneralService;

struct channelServiceStruct {
  BLEService *channel;
  BLEIntCharacteristic *numLEDs;
  BLEIntCharacteristic *stripType;
  BLEIntCharacteristic *position;
  Adafruit_NeoPixel *strip;
};
typedef struct channelServiceStruct ChannelService;

struct commandServiceStruct {
  BLEService *command;
  BLEIntCharacteristic *primaryColor;
  BLEIntCharacteristic *secondaryColor;
  BLEIntCharacteristic *delay;
  BLEIntCharacteristic *animation;
  BLEByteCharacteristic *usedChannels;
};
typedef struct commandServiceStruct CommandService;

struct servicesStruct {
  GeneralService generalService;
  char channelServiceUUIDs[MAX_CHANNELS][UUID_LENGTH];
  ChannelService channelServices[MAX_CHANNELS];
  char commandServiceUUIDs[MAX_COMMANDS][UUID_LENGTH];
  CommandService commandServices[MAX_COMMANDS];
};
typedef struct servicesStruct Services;

struct channelStruct {
  int numLEDs;
  int stripType;
  int rowPosition;
  int colPosition;
};
typedef struct channelStruct Channel;

struct commandStruct {
  int primaryRed;
  int primaryGreen;
  int primaryBlue;
  int secondaryRed;
  int secondaryGreen;
  int secondaryBlue;
  int delay;
  int animation;
  bool usedChannels[MAX_CHANNELS];
};
typedef struct commandStruct Command;

struct commandSetStruct {
  bool lightsOn;
  Channel channels[MAX_CHANNELS];
  Command commands[MAX_COMMANDS];
  int currentCommandIndex;
};
typedef struct commandSetStruct CommandSet;

Services services;
CommandSet commandSet;
FlashStorage(flash_store, CommandSet);

void setup() {
  Serial.begin(9600);
  //while(!Serial);//temporary stops running until serial connection is made
  Serial.println("Starting BLE");
  while (!BLE.begin()) {
    Serial.println("Waiting for BLE to start");
    delay(1);
  }
  
  BLE.setDeviceName("SquadGlowController");
  BLE.setLocalName("SquadGlowController");
  setupGeneralService();
  setupChannelServices();
  setupCommandServices();
  //commandSet = flash_store.read();
  applyTestingCommandSet();
  if(isCommandUsed(0) == true){//no saved commandSet
    Serial.println("Saved commandSet found");
    updateServiceValues();//update service values to match commandSet
    describeCommandSet();
  }else{//found saved commandSet
    Serial.println("No saved commandSet found");
    initializeServiceValues();//sets all characteristics to 0
    updateCommandSet();//builds commandset based on service values
    applyTestingCommandSet();//temp
    updateServiceValues();//temp
    describeCommandSet();
  }
  BLE.setAdvertisedService(*services.generalService.general);
  BLE.advertise();//I think only 1 is advertised at a time
  Serial.println("Ready!");
}

void loop() {
    BLE.poll();//get new BLE events
    if(commandSet.lightsOn == true){
      Serial.println("Lights are on");
      runCommand(commandSet.currentCommandIndex);//run active command
    }else{
      Serial.println("Lights are off");
    }
    /*BLEDevice central = BLE.central();
    if(central.connected()){
      Serial.println("Central Connected");
    }else{
      Serial.println("Central Disconnected");     
      if(commandSet.lightsOn == true){
        //Serial.println("Lights are on");
        runCommand(commandSet.currentCommandIndex);//run active command
      }else{
        //Serial.println("Lights are off");
        //function to set all leds to off
      }
    }*/
}
void updatePrimaryColors(int commandIndex, int value){
  Serial.println("Start Updating Primary Colors");
  ArrayToInteger readConverter;
  readConverter.integer = value;
  commandSet.commands[commandIndex].primaryRed = readConverter.array[0];
  commandSet.commands[commandIndex].primaryGreen = readConverter.array[1];
  commandSet.commands[commandIndex].primaryBlue = readConverter.array[2];
  Serial.println("Finish updating colors");
}

void updateSecondaryColors(int commandIndex, int value){
  Serial.println("Start Updating Primary Colors");
  ArrayToInteger readConverter;
  readConverter.integer = value;
  commandSet.commands[commandIndex].secondaryRed = readConverter.array[0];
  commandSet.commands[commandIndex].secondaryGreen = readConverter.array[1];
  commandSet.commands[commandIndex].secondaryBlue = readConverter.array[2];
  Serial.println("Finish updating colors");
}

int getPrimaryColorValue(int commandIndex){
  Serial.println("Start getting color");
  ArrayToInteger writeConverter;
  writeConverter.array[0] = commandSet.commands[commandIndex].primaryRed;
  writeConverter.array[1] = commandSet.commands[commandIndex].primaryGreen;
  writeConverter.array[2] = commandSet.commands[commandIndex].primaryBlue;
  writeConverter.array[3] = 0;
  Serial.println("Finish getting color");
  return writeConverter.integer;
}

int getSecondaryColorValue(int commandIndex){
  Serial.println("Start getting color");
  ArrayToInteger writeConverter;
  writeConverter.array[0] = commandSet.commands[commandIndex].secondaryRed;
  writeConverter.array[1] = commandSet.commands[commandIndex].secondaryGreen;
  writeConverter.array[2] = commandSet.commands[commandIndex].secondaryBlue;
  writeConverter.array[3] = 0;
  Serial.println("Finish getting color");
  return writeConverter.integer;
}

void updateUsedChannels(int commandIndex, byte value){
  Serial.println("Start updating used channels");
  byte mask = 1;
  byte index = 0;
  for (mask = 00000001; mask>0; mask <<= 1) { //iterate through bit mask
    if (value & mask){ // if bitwise AND resolves to true
      commandSet.commands[commandIndex].usedChannels[index] = true;
      index++;
    }
    else{ //if bitwise and resolves to false
      commandSet.commands[commandIndex].usedChannels[index] = false;
      index++;
    }
  }
  Serial.println("Finish updating used channels");
}

void applyTestingCommandSet(){
  Serial.println("Applying Testing CommandSet");
  commandSet.lightsOn = true;
  commandSet.channels[0].numLEDs = 100;
  commandSet.channels[0].stripType = 1;
  commandSet.channels[0].rowPosition = 0;
  commandSet.channels[0].colPosition = 1;
  commandSet.channels[1].numLEDs = 100;
  commandSet.channels[1].stripType = 1;
  commandSet.channels[1].rowPosition = 0;
  commandSet.channels[1].colPosition = 1;
  commandSet.channels[2].numLEDs = 100;
  commandSet.channels[2].stripType = 1;
  commandSet.channels[2].rowPosition = 0;
  commandSet.channels[2].colPosition = 1;
  commandSet.channels[3].numLEDs = 100;
  commandSet.channels[3].stripType = 1;
  commandSet.channels[3].rowPosition = 0;
  commandSet.channels[3].colPosition = 1;
  commandSet.channels[4].numLEDs = 100;
  commandSet.channels[4].stripType = 1;
  commandSet.channels[4].rowPosition = 0;
  commandSet.channels[4].colPosition = 1;
  commandSet.channels[5].numLEDs = 100;
  commandSet.channels[5].stripType = 1;
  commandSet.channels[5].rowPosition = 0;
  commandSet.channels[5].colPosition = 1;
  commandSet.channels[6].numLEDs = 100;
  commandSet.channels[6].stripType = 1;
  commandSet.channels[6].rowPosition = 0;
  commandSet.channels[6].colPosition = 1;
  commandSet.channels[7].numLEDs = 100;
  commandSet.channels[7].stripType = 1;
  commandSet.channels[7].rowPosition = 0;
  commandSet.channels[7].colPosition = 1;
  
  commandSet.commands[0].primaryRed = 255;
  commandSet.commands[0].primaryGreen = 0;
  commandSet.commands[0].primaryBlue = 255;
  commandSet.commands[0].secondaryRed = 0;
  commandSet.commands[0].secondaryGreen = 255;
  commandSet.commands[0].secondaryBlue = 255;
  commandSet.commands[0].delay = 0;
  commandSet.commands[0].animation = 0;
  commandSet.commands[0].usedChannels[0] = true;
  commandSet.commands[0].usedChannels[1] = true;
  commandSet.commands[0].usedChannels[2] = true;
  commandSet.commands[0].usedChannels[3] = true;
  commandSet.commands[0].usedChannels[4] = true;
  commandSet.commands[0].usedChannels[5] = true;
  commandSet.commands[0].usedChannels[6] = true;
  commandSet.commands[0].usedChannels[7] = true;
}

void initializeServiceValues() {
  Serial.println("Initializing Service Values");
  services.generalService.save->writeValue(false);
  services.generalService.on->writeValue(true);
  for(int i=0; i<MAX_CHANNELS; i++){
    services.channelServices[i].numLEDs->writeValue(0); 
    services.channelServices[i].stripType->writeValue(0);
    services.channelServices[i].position->writeValue(0);
  }
  
  for(int i=0; i<MAX_COMMANDS; i++){
    services.commandServices[i].primaryColor->writeValue(getPrimaryColorValue(i));
    services.commandServices[i].secondaryColor->writeValue(getSecondaryColorValue(i));
    services.commandServices[i].delay->writeValue(0);
    services.commandServices[i].animation->writeValue(0);
    services.commandServices[i].usedChannels->writeValue(getUsedChannelsByte(i));
  }
}

void updateServiceValues(){
  Serial.println("Updating Service Values");
  services.generalService.on->writeValue(commandSet.lightsOn);
  for(int i=0; i<MAX_CHANNELS; i++){
    services.channelServices[i].numLEDs->writeValue(commandSet.channels[i].numLEDs); 
    services.channelServices[i].stripType->writeValue(commandSet.channels[i].stripType);
    services.channelServices[i].position->writeValue(getPositionValue(i));
  }
  
  for(int i=0; i<MAX_COMMANDS; i++){
    services.commandServices[i].primaryColor->writeValue(getPrimaryColorValue(i)); //build propper color value
    services.commandServices[i].secondaryColor->writeValue(getSecondaryColorValue(i)); //build propper color value
    services.commandServices[i].delay->writeValue(commandSet.commands[i].delay);
    services.commandServices[i].animation->writeValue(commandSet.commands[i].animation);
    services.commandServices[i].usedChannels->writeValue(getUsedChannelsByte(i));
  }
}

void updatePositions(int channelIndex, int value){
  Serial.println("Start updating positions");
  ArrayToInteger readConverter;
  readConverter.integer = value;
  commandSet.channels[channelIndex].rowPosition = readConverter.array[0];
  commandSet.channels[channelIndex].colPosition = readConverter.array[1];
  Serial.println("Finsih updating positions");
}

int getPositionValue(int channelIndex){
  Serial.println("Start getting position");
  ArrayToInteger writeConverter;
  writeConverter.array[0] = commandSet.channels[channelIndex].rowPosition;
  writeConverter.array[1] = commandSet.channels[channelIndex].colPosition;
  writeConverter.array[2] = 0;
  writeConverter.array[3] = 0;
  Serial.println("Finsih getting position");
  return writeConverter.integer;
}

byte getUsedChannelsByte(int commandIndex){ //need to actually do this
  Serial.println("Start getting used channels");
  int returnValue = 0;
  int bitValues[8] = {1,2,4,8,16,32,64,128};
  for(int i=0; i<MAX_CHANNELS; i++){
      if(commandSet.commands[commandIndex].usedChannels[i] == true){
        returnValue += bitValues[i];
      }
  }
  Serial.println("Finish getting used channels");
  return returnValue;
}


Adafruit_NeoPixel* getStripDefinition(int value, int channelIndex){
  Serial.println("Start getting strip definition");
  ArrayToInteger readConverter;
  readConverter.integer = value;
  int pixelOrder = 0;
  int pixelType = 0;
  
  switch(readConverter.array[0]){
    case 1:
      pixelType = NEO_KHZ800;
      break;
    case 2:
      pixelType = NEO_KHZ400;
      break;
     default:
      pixelType = NEO_KHZ800;
      break;
  }

  switch(readConverter.array[1]){
    case 0:
      pixelOrder = NEO_RGB;
      break;
    case 1:
      pixelOrder = NEO_RBG;
      break;
    case 2:
      pixelOrder = NEO_BRG;
      break;
    case 3:
      pixelOrder = NEO_BGR;
      break;
    case 4:
      pixelOrder = NEO_GBR;
      break;
    case 5:
      pixelOrder = NEO_GRB;
      break;
    //may add the RGBW pixel orders at some point
    default:
      pixelOrder = NEO_RGB;
  }
  int pixelFormat = pixelOrder + pixelType;
  Serial.println("Finsih getting strip definition");
  return new Adafruit_NeoPixel(MAX_LEDS_PER_CHANNEL, outputPins[channelIndex], pixelFormat);
}


void saveCommandSetHandler(BLEDevice central, BLECharacteristic characteristic){
  bool value = services.generalService.save->value();
  if(value == true){
      Serial.println("Saving commandSet");
      flash_store.write(commandSet);
      services.generalService.save->writeValue(false);
  }else{
    Serial.println("Save was updated but value is false");
  }
}

void updateCommandSetHandler(BLEDevice central, BLECharacteristic characteristic){
  updateCommandSet();
  describeCommandSet();
}

void describeCommandSet(){
  Serial.println("Describing Current CommandSet: ");
  Serial.print("Light Status: ");
  Serial.println(commandSet.lightsOn);
  Serial.print("Current Command Index: ");
  Serial.println(commandSet.currentCommandIndex);
  
  for(int i=0; i<MAX_CHANNELS; i++){
    Serial.println("{");
    Serial.print("Channel ID: ");
    Serial.println(i);
    Serial.print("Number LEDs: ");
    Serial.println(commandSet.channels[i].numLEDs);
    Serial.print("Strip Type: ");
    Serial.println(commandSet.channels[i].stripType);
    Serial.print("Position: ");
    Serial.print(commandSet.channels[i].rowPosition);
    Serial.print(":");
    Serial.println(commandSet.channels[i].colPosition);
    Serial.println("}");
  }
  
  for(int i=0; i<MAX_COMMANDS; i++){
    Serial.println("{");
    Serial.print("Command ID: ");
    Serial.println(i);
    Serial.print("Primary Red: ");
    Serial.println(commandSet.commands[i].primaryRed);
    Serial.print("Primary Green: ");
    Serial.println(commandSet.commands[i].primaryGreen);
    Serial.print("Primary Blue: ");
    Serial.println(commandSet.commands[i].primaryBlue);
    Serial.print("Secondary Red: ");
    Serial.println(commandSet.commands[i].secondaryRed);
    Serial.print("Secondary Green: ");
    Serial.println(commandSet.commands[i].secondaryGreen);
    Serial.print("Secondary Blue: ");
    Serial.println(commandSet.commands[i].secondaryBlue);
    Serial.print("Delay: ");
    Serial.println(commandSet.commands[i].delay);
    Serial.print("Animation: ");
    Serial.println(commandSet.commands[i].animation);
    Serial.println("Used Channels: ");
    for(int j=0; j<MAX_CHANNELS; j++){
      if(commandSet.commands[i].usedChannels[j] == true){
        Serial.print("Used: ");
        Serial.println(j);
      }else{
        Serial.print("Not Used: ");
        Serial.println(j);
      }
    }
    Serial.println("}");
  }
}

void updateCommandSet(){
  Serial.println("Updating commandSet");
  commandSet.currentCommandIndex = 0;
  commandSet.lightsOn = services.generalService.on->value();
  for(int i=0; i<MAX_CHANNELS; i++){
    int numLEDsSize = services.channelServices[i].numLEDs->valueSize();
    byte numLEDsBuffer[numLEDsSize];
    services.channelServices[i].numLEDs->readValue(numLEDsBuffer, numLEDsSize);
    int combinedValue = numLEDsBuffer[0] + numLEDsBuffer[1];
    commandSet.channels[i].numLEDs = combinedValue;
    
    commandSet.channels[i].stripType = services.channelServices[i].stripType->value();
    delete services.channelServices[i].strip;
    services.channelServices[i].strip = getStripDefinition(services.channelServices[i].stripType->value(), i);
    
    updatePositions(i, services.channelServices[i].position->value());
  }
  
  for(int i=0; i<MAX_COMMANDS; i++){ 
    updatePrimaryColors(i, services.commandServices[i].primaryColor->value());
    updateSecondaryColors(i, services.commandServices[i].secondaryColor->value());
    commandSet.commands[i].delay = services.commandServices[i].delay->value();
    commandSet.commands[i].animation = services.commandServices[i].animation->value();
    updateUsedChannels(i, services.commandServices[i].usedChannels->value());
  }
}

void runCommand(int commandIndex){
  switch(commandSet.commands[commandIndex].animation){
    case 0:
      runStaticCommand(commandIndex);
      break;
  }
}

bool isCommandUsed(int commandIndex){
  bool found = false;
  for(int i=0; i<MAX_CHANNELS; i++){
    if(commandSet.commands[commandIndex].usedChannels[i] == true){
      found = true;
    }
  }
  return found;
}

void runStaticCommand(int commandIndex){
  //Serial.println("Running Static Command");
  if(isCommandUsed(commandIndex) == true){
    for(int i=0; i<MAX_CHANNELS; i++){
      if(commandSet.commands[commandIndex].usedChannels[i] == true && commandSet.channels[i].numLEDs > 0){
        uint32_t color = services.channelServices[i].strip->Color(commandSet.commands[commandIndex].primaryRed, commandSet.commands[commandIndex].primaryGreen, commandSet.commands[commandIndex].primaryBlue);
        
        for(int j=0; j<commandSet.channels[i].numLEDs; j++){
          services.channelServices[i].strip->setPixelColor(j, color);
        }
        services.channelServices[i].strip->show();
        
        Serial.print("Color Applied to channel ");
        Serial.println(i);
      }
    }
    BLE.poll(commandSet.commands[commandIndex].delay);//get new BLE events
  }
}

void setupGeneralService(){
    services.generalService.general = new BLEService(generalUUIDs[0]);
    services.generalService.on = new BLEBoolCharacteristic(generalUUIDs[1], BLERead | BLEWrite);// | BLENotify
    services.generalService.save = new BLEBoolCharacteristic(generalUUIDs[2], BLERead | BLEWrite);
    
    services.generalService.on->setEventHandler(BLEWritten, updateCommandSetHandler);
    services.generalService.save->setEventHandler(BLEWritten, saveCommandSetHandler);
    
    services.generalService.general->addCharacteristic(*services.generalService.on);
    services.generalService.general->addCharacteristic(*services.generalService.save);
    
    BLE.addService(*services.generalService.general);
}

void setupChannelServices(){
  Serial.println("Creating Channel Services");
  for(int i=0; i<MAX_CHANNELS; i++){
    services.channelServices[i].strip = getStripDefinition(commandSet.channels[i].stripType, i);
    services.channelServices[i].strip->show();
    pinMode(outputPins[i], OUTPUT);
    digitalWrite(outputPins[i], LOW);

    services.channelServices[i].channel = new BLEService(channelUUIDs[i]);
    services.channelServices[i].numLEDs = new BLEIntCharacteristic(channelNumLEDsUUIDs[i], BLERead | BLEWrite);// | BLENotify
    services.channelServices[i].stripType = new BLEIntCharacteristic(channelStripTypeUUIDs[i], BLERead | BLEWrite);
    services.channelServices[i].position = new BLEIntCharacteristic(channelPositionUUIDs[i], BLERead | BLEWrite);
    
    services.channelServices[i].numLEDs->setEventHandler(BLEWritten, updateCommandSetHandler);
    services.channelServices[i].stripType->setEventHandler(BLEWritten, updateCommandSetHandler);
    services.channelServices[i].position->setEventHandler(BLEWritten, updateCommandSetHandler);
    
    services.channelServices[i].channel->addCharacteristic(*services.channelServices[i].numLEDs);
    services.channelServices[i].channel->addCharacteristic(*services.channelServices[i].stripType);
    services.channelServices[i].channel->addCharacteristic(*services.channelServices[i].position);
    
    BLE.addService(*services.channelServices[i].channel);
  }
}

void setupCommandServices(){
  Serial.println("Creating Command Services");
  for(int i=0; i<MAX_COMMANDS; i++){
    services.commandServices[i].command = new BLEService(commandUUIDs[i]);
    
    services.commandServices[i].primaryColor = new BLEIntCharacteristic(commandPrimaryColorUUIDs[i], BLERead | BLEWrite);
    services.commandServices[i].secondaryColor = new BLEIntCharacteristic(commandSecondaryColorUUIDs[i], BLERead | BLEWrite);
    services.commandServices[i].delay = new BLEIntCharacteristic(commandDelayUUIDs[i], BLERead | BLEWrite);
    services.commandServices[i].animation = new BLEIntCharacteristic(commandAnimationUUIDs[i], BLERead | BLEWrite);
    services.commandServices[i].usedChannels = new BLEByteCharacteristic(commandChannelToggleUUIDs[i], BLERead | BLEWrite);
    
    services.commandServices[i].primaryColor->setEventHandler(BLEWritten, updateCommandSetHandler);
    services.commandServices[i].secondaryColor->setEventHandler(BLEWritten, updateCommandSetHandler);
    services.commandServices[i].delay->setEventHandler(BLEWritten, updateCommandSetHandler);
    services.commandServices[i].animation->setEventHandler(BLEWritten, updateCommandSetHandler);
    services.commandServices[i].usedChannels->setEventHandler(BLEWritten, updateCommandSetHandler);

    services.commandServices[i].command->addCharacteristic(*services.commandServices[i].primaryColor);
    services.commandServices[i].command->addCharacteristic(*services.commandServices[i].secondaryColor);
    services.commandServices[i].command->addCharacteristic(*services.commandServices[i].delay);
    services.commandServices[i].command->addCharacteristic(*services.commandServices[i].animation);
    services.commandServices[i].command->addCharacteristic(*services.commandServices[i].usedChannels);
      
    BLE.addService(*services.commandServices[i].command);
  }
}