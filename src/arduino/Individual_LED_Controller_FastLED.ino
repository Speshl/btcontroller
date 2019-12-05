//FASTLED doesn't appear to be supported yet on the arduino nano 33 IOT
#include <FlashStorage.h>
#include <ArduinoBLE.h>
#include <FastLED.h>

#define MAX_LEDS_PER_CHANNEL 20
#define MAX_COMMANDS 5
#define MAX_CHANNELS 5
#define UUID_LENGTH 37

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
  "20a734d0-1e9a-4da4-bc33-b914db19b956"
  //"9452f330-e8ad-4db0-ab2b-779c57765093",
  //"aa2b7ad0-7721-4d79-895f-298a8addb129"
};

const char channelNumLEDsUUIDs[MAX_CHANNELS][UUID_LENGTH] = {
  "601e09c1-80c6-48b6-af07-8b3e972db14b",
  "00ffb031-9cc8-4f4e-80d8-a5d0391e5060",
  "9cac5611-3cce-4b5b-8f09-7befa63be3c9",
  "12d2c3e1-1c17-4ae8-9307-5f400dc5c6e4",
  "20a734d1-1e9a-4da4-bc33-b914db19b956"
  //"9452f331-e8ad-4db0-ab2b-779c57765093",
  //"aa2b7ad1-7721-4d79-895f-298a8addb129"
};

const char channelStripTypeUUIDs[MAX_CHANNELS][UUID_LENGTH] = {
  "601e09c2-80c6-48b6-af07-8b3e972db14b",
  "00ffb032-9cc8-4f4e-80d8-a5d0391e5060",
  "9cac5612-3cce-4b5b-8f09-7befa63be3c9",
  "12d2c3e2-1c17-4ae8-9307-5f400dc5c6e4",
  "20a734d2-1e9a-4da4-bc33-b914db19b956"
  //"9452f332-e8ad-4db0-ab2b-779c57765093",
  //"aa2b7ad2-7721-4d79-895f-298a8addb129"
};

const char commandUUIDs[MAX_COMMANDS][UUID_LENGTH] = {
  "f1ca8500-6f92-4c90-adcf-86932eae494e",
  "b7fa5bb0-83a9-4c12-9386-44915994d3e6",
  "9d07ec50-cd1f-4e0b-9b66-e2c543b564bf",
  "75ba4c90-3379-457c-a94b-dc106fccf5b7",
  "1a4d00b0-5397-441d-b161-c8755fa8a7f7"
};

const char commandRedUUIDs[MAX_COMMANDS][UUID_LENGTH] = {
  "f1ca8501-6f92-4c90-adcf-86932eae494e",
  "b7fa5bb1-83a9-4c12-9386-44915994d3e6",
  "9d07ec51-cd1f-4e0b-9b66-e2c543b564bf",
  "75ba4c91-3379-457c-a94b-dc106fccf5b7",
  "1a4d00b1-5397-441d-b161-c8755fa8a7f7"
};

const char commandGreenUUIDs[MAX_COMMANDS][UUID_LENGTH] = {
  "f1ca8502-6f92-4c90-adcf-86932eae494e",
  "b7fa5bb2-83a9-4c12-9386-44915994d3e6",
  "9d07ec52-cd1f-4e0b-9b66-e2c543b564bf",
  "75ba4c92-3379-457c-a94b-dc106fccf5b7",
  "1a4d00b2-5397-441d-b161-c8755fa8a7f7"
};

const char commandBlueUUIDs[MAX_COMMANDS][UUID_LENGTH] = {
  "f1ca8503-6f92-4c90-adcf-86932eae494e",
  "b7fa5bb3-83a9-4c12-9386-44915994d3e6",
  "9d07ec53-cd1f-4e0b-9b66-e2c543b564bf",
  "75ba4c93-3379-457c-a94b-dc106fccf5b7",
  "1a4d00b3-5397-441d-b161-c8755fa8a7f7"
};

const char commandDelayUUIDs[MAX_COMMANDS][UUID_LENGTH] = {
  "f1ca8504-6f92-4c90-adcf-86932eae494e",
  "b7fa5bb4-83a9-4c12-9386-44915994d3e6",
  "9d07ec54-cd1f-4e0b-9b66-e2c543b564bf",
  "75ba4c94-3379-457c-a94b-dc106fccf5b7",
  "1a4d00b4-5397-441d-b161-c8755fa8a7f7"
};

const char commandAnimationUUIDs[MAX_COMMANDS][UUID_LENGTH] = {
  "f1ca8505-6f92-4c90-adcf-86932eae494e",
  "b7fa5bb5-83a9-4c12-9386-44915994d3e6",
  "9d07ec55-cd1f-4e0b-9b66-e2c543b564bf",
  "75ba4c95-3379-457c-a94b-dc106fccf5b7",
  "1a4d00b5-5397-441d-b161-c8755fa8a7f7"
};

/*const char commandUsedChannelsUUIDs[MAX_COMMANDS][UUID_LENGTH] = {
  "f1ca8506-6f92-4c90-adcf-86932eae494e",
  "b7fa5bb6-83a9-4c12-9386-44915994d3e6",
  "9d07ec56-cd1f-4e0b-9b66-e2c543b564bf",
  "75ba4c96-3379-457c-a94b-dc106fccf5b7",
  "1a4d00b6-5397-441d-b161-c8755fa8a7f7"
};*/

const char commandChannelToggleUUIDs[MAX_COMMANDS][MAX_CHANNELS][UUID_LENGTH] = {
  {
    "f1ca8507-6f92-4c90-adcf-86932eae494e",
    "f1ca8508-6f92-4c90-adcf-86932eae494e",
    "f1ca8509-6f92-4c90-adcf-86932eae494e",
    "f1ca850a-6f92-4c90-adcf-86932eae494e",
    "f1ca850b-6f92-4c90-adcf-86932eae494e"
    //"f1ca850c-6f92-4c90-adcf-86932eae494e",
    //"f1ca850d-6f92-4c90-adcf-86932eae494e"
  },
  {
    "b7fa5bb7-83a9-4c12-9386-44915994d3e6",
    "b7fa5bb8-83a9-4c12-9386-44915994d3e6",
    "b7fa5bb9-83a9-4c12-9386-44915994d3e6",
    "b7fa5bba-83a9-4c12-9386-44915994d3e6",
    "b7fa5bbb-83a9-4c12-9386-44915994d3e6"
    //"b7fa5bbc-83a9-4c12-9386-44915994d3e6",
    //"b7fa5bbd-83a9-4c12-9386-44915994d3e6"
  },{
    "9d07ec57-cd1f-4e0b-9b66-e2c543b564bf",
    "9d07ec58-cd1f-4e0b-9b66-e2c543b564bf",
    "9d07ec59-cd1f-4e0b-9b66-e2c543b564bf",
    "9d07ec5a-cd1f-4e0b-9b66-e2c543b564bf",
    "9d07ec5b-cd1f-4e0b-9b66-e2c543b564bf"
    //"9d07ec5c-cd1f-4e0b-9b66-e2c543b564bf",
    //"9d07ec5d-cd1f-4e0b-9b66-e2c543b564bf"
  },{
    "75ba4c97-3379-457c-a94b-dc106fccf5b7",
    "75ba4c98-3379-457c-a94b-dc106fccf5b7",
    "75ba4c99-3379-457c-a94b-dc106fccf5b7",
    "75ba4c9a-3379-457c-a94b-dc106fccf5b7",
    "75ba4c9b-3379-457c-a94b-dc106fccf5b7"
    //"75ba4c9c-3379-457c-a94b-dc106fccf5b7",
    //"75ba4c9d-3379-457c-a94b-dc106fccf5b7"
  },{
    "1a4d00b7-5397-441d-b161-c8755fa8a7f7",
    "1a4d00b8-5397-441d-b161-c8755fa8a7f7",
    "1a4d00b9-5397-441d-b161-c8755fa8a7f7",
    "1a4d00ba-5397-441d-b161-c8755fa8a7f7",
    "1a4d00bb-5397-441d-b161-c8755fa8a7f7"
    //"1a4d00bc-5397-441d-b161-c8755fa8a7f7",
    //"1a4d00bd-5397-441d-b161-c8755fa8a7f7"
  }
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
  CRGB leds[MAX_LEDS_PER_CHANNEL];
};
typedef struct channelServiceStruct ChannelService;

struct commandServiceStruct {
  BLEService *command;
  BLEIntCharacteristic *red;
  BLEIntCharacteristic *green;
  BLEIntCharacteristic *blue;
  BLEIntCharacteristic *delay;
  BLEIntCharacteristic *animation;
  BLEBoolCharacteristic *usedChannels[MAX_CHANNELS];
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
};
typedef struct channelStruct Channel;

struct commandStruct {
  int red;
  int green;
  int blue;
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
  while(!Serial);//temporary stops running until serial connection is made
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
    BLEDevice central = BLE.central();
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
    }
}

void applyTestingCommandSet(){
  Serial.println("Applying Testing CommandSet");
  commandSet.lightsOn = true;
  commandSet.channels[0].numLEDs = 10;
  commandSet.channels[0].stripType = 1;
  commandSet.channels[1].numLEDs = 10;
  commandSet.channels[1].stripType = 1;
  commandSet.channels[2].numLEDs = 10;
  commandSet.channels[2].stripType = 1;
  commandSet.channels[3].numLEDs = 10;
  commandSet.channels[3].stripType = 1;
  commandSet.channels[4].numLEDs = 10;
  commandSet.channels[4].stripType = 1;
  
  commandSet.commands[0].red = 255;
  commandSet.commands[0].green = 0;
  commandSet.commands[0].blue = 255;
  commandSet.commands[0].delay = 0;
  commandSet.commands[0].animation = 0;
  commandSet.commands[0].usedChannels[0] = true;
  commandSet.commands[0].usedChannels[1] = true;
  commandSet.commands[0].usedChannels[2] = true;
  commandSet.commands[0].usedChannels[3] = true;
  commandSet.commands[0].usedChannels[4] = true;
}

void initializeServiceValues() {
  Serial.println("Initializing Service Values");
  services.generalService.save->writeValue(false);
  services.generalService.on->writeValue(true);
  for(int i=0; i<MAX_CHANNELS; i++){
    services.channelServices[i].numLEDs->writeValue(0); 
    services.channelServices[i].stripType->writeValue(0);
  }
  
  for(int i=0; i<MAX_COMMANDS; i++){
    services.commandServices[i].red->writeValue(0);
    services.commandServices[i].green->writeValue(0);
    services.commandServices[i].blue->writeValue(0);
    services.commandServices[i].delay->writeValue(0);
    services.commandServices[i].animation->writeValue(0);
    for(int j=0; j<MAX_CHANNELS; j++){
      services.commandServices[i].usedChannels[j]->writeValue(false);
    }
  }
}

void updateServiceValues(){
  Serial.println("Updating Service Values");
  services.generalService.on->writeValue(commandSet.lightsOn);
  for(int i=0; i<MAX_CHANNELS; i++){
    services.channelServices[i].numLEDs->writeValue(commandSet.channels[i].numLEDs); 
    services.channelServices[i].stripType->writeValue(commandSet.channels[i].stripType);
  }
  
  for(int i=0; i<MAX_COMMANDS; i++){
    services.commandServices[i].red->writeValue(commandSet.commands[i].red);
    services.commandServices[i].green->writeValue(commandSet.commands[i].green);
    services.commandServices[i].blue->writeValue(commandSet.commands[i].blue);
    services.commandServices[i].delay->writeValue(commandSet.commands[i].delay);
    services.commandServices[i].animation->writeValue(commandSet.commands[i].animation);

    for(int j=0; j<MAX_CHANNELS; j++){
      services.commandServices[i].usedChannels[j]->writeValue(commandSet.commands[i].usedChannels[j]);
    }
  }
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
    Serial.println("}");
  }
  
  for(int i=0; i<MAX_COMMANDS; i++){
    Serial.println("{");
    Serial.print("Command ID: ");
    Serial.println(i);
    Serial.print("Red: ");
    Serial.println(commandSet.commands[i].red);
    Serial.print("Green: ");
    Serial.println(commandSet.commands[i].green);
    Serial.print("Blue: ");
    Serial.println(commandSet.commands[i].blue);
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
    
    int stripTypeSize = services.channelServices[i].stripType->valueSize();
    byte stripTypeBuffer[stripTypeSize];
    services.channelServices[i].stripType->readValue(stripTypeBuffer, stripTypeSize);
    commandSet.channels[i].stripType = stripTypeBuffer[0];
  }
  
  for(int i=0; i<MAX_COMMANDS; i++){
    commandSet.commands[i].red = services.commandServices[i].red->value();
    commandSet.commands[i].green = services.commandServices[i].green->value();
    commandSet.commands[i].blue = services.commandServices[i].blue->value();
    commandSet.commands[i].delay = services.commandServices[i].delay->value();
    commandSet.commands[i].animation = services.commandServices[i].animation->value();

    for(int j=0; j<MAX_CHANNELS; j++){
      bool tempValue = services.commandServices[i].usedChannels[j]->value();
      if(tempValue == true){
        commandSet.commands[i].usedChannels[j] = true;
      }else{
        commandSet.commands[i].usedChannels[j] = false;
      }
    }
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
  CRGB staticColor = CRGB(
    commandSet.commands[commandIndex].red,
    commandSet.commands[commandIndex].green,
    commandSet.commands[commandIndex].blue
  );
  
  if(isCommandUsed(commandIndex) == true){
    for(int i=0; i<MAX_CHANNELS; i++){
      if(commandSet.commands[commandIndex].usedChannels[i] == true && commandSet.channels[i].numLEDs > 0){
        fill_solid(
          services.channelServices[i].leds,
          commandSet.channels[i].numLEDs,
          staticColor
        );
        //FastLED[i].showLeds(255);//apply color
        Serial.print("Color Applied to channel ");
        Serial.println(i);
      }
    }
    //FastLED.show();
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
    fill_solid(services.channelServices[i].leds, MAX_LEDS_PER_CHANNEL, CRGB::Red);
    switch(i){
      case 0:
        FastLED.addLeds<WS2811, 3, GRB>(services.channelServices[i].leds, MAX_LEDS_PER_CHANNEL);//pin number and strip type hardcoded for now, also always using max led instead of specified characteristic, can specify GRB after pin number 
        break;
      case 1:
        FastLED.addLeds<WS2811, 5, GRB>(services.channelServices[i].leds, MAX_LEDS_PER_CHANNEL);//pin number and strip type hardcoded for now, also always using max led instead of specified characteristic
        break;
      case 2:
        FastLED.addLeds<WS2811, 6, GRB>(services.channelServices[i].leds, MAX_LEDS_PER_CHANNEL);//pin number and strip type hardcoded for now, also always using max led instead of specified characteristic
        break;
      case 3:
        FastLED.addLeds<WS2811, 9, GRB>(services.channelServices[i].leds, MAX_LEDS_PER_CHANNEL);//pin number and strip type hardcoded for now, also always using max led instead of specified characteristic
        break;
      case 4:
        FastLED.addLeds<WS2811, 10, GRB>(services.channelServices[i].leds, MAX_LEDS_PER_CHANNEL);//pin number and strip type hardcoded for now, also always using max led instead of specified characteristic
        break;
    } 
    services.channelServices[i].channel = new BLEService(channelUUIDs[i]);
    services.channelServices[i].numLEDs = new BLEIntCharacteristic(channelNumLEDsUUIDs[i], BLERead | BLEWrite);// | BLENotify
    services.channelServices[i].stripType = new BLEIntCharacteristic(channelStripTypeUUIDs[i], BLERead | BLEWrite);
    
    services.channelServices[i].numLEDs->setEventHandler(BLEWritten, updateCommandSetHandler);
    services.channelServices[i].stripType->setEventHandler(BLEWritten, updateCommandSetHandler);
    
    services.channelServices[i].channel->addCharacteristic(*services.channelServices[i].numLEDs);
    services.channelServices[i].channel->addCharacteristic(*services.channelServices[i].stripType);
    
    BLE.addService(*services.channelServices[i].channel);
  }
}

void setupCommandServices(){
  Serial.println("Creating Command Services");
  for(int i=0; i<MAX_COMMANDS; i++){
    services.commandServices[i].command = new BLEService(commandUUIDs[i]);
    services.commandServices[i].red = new BLEIntCharacteristic(commandRedUUIDs[i], BLERead | BLEWrite);
    services.commandServices[i].green = new BLEIntCharacteristic(commandGreenUUIDs[i], BLERead | BLEWrite);
    services.commandServices[i].blue = new BLEIntCharacteristic(commandBlueUUIDs[i], BLERead | BLEWrite);
    services.commandServices[i].delay = new BLEIntCharacteristic(commandDelayUUIDs[i], BLERead | BLEWrite);
    services.commandServices[i].animation = new BLEIntCharacteristic(commandAnimationUUIDs[i], BLERead | BLEWrite);

    for(int j=0; j<MAX_CHANNELS; j++){
      services.commandServices[i].usedChannels[j] = new BLEBoolCharacteristic(commandChannelToggleUUIDs[i][j], BLERead | BLEWrite);
    }
    
    services.commandServices[i].red->setEventHandler(BLEWritten, updateCommandSetHandler);
    services.commandServices[i].green->setEventHandler(BLEWritten, updateCommandSetHandler);
    services.commandServices[i].blue->setEventHandler(BLEWritten, updateCommandSetHandler);
    services.commandServices[i].delay->setEventHandler(BLEWritten, updateCommandSetHandler);
    services.commandServices[i].animation->setEventHandler(BLEWritten, updateCommandSetHandler);

    for(int j=0; j<MAX_CHANNELS; j++){
      services.commandServices[i].usedChannels[j]->setEventHandler(BLEWritten, updateCommandSetHandler);
    }

    services.commandServices[i].command->addCharacteristic(*services.commandServices[i].red);
    services.commandServices[i].command->addCharacteristic(*services.commandServices[i].green);
    services.commandServices[i].command->addCharacteristic(*services.commandServices[i].blue);
    services.commandServices[i].command->addCharacteristic(*services.commandServices[i].delay);
    services.commandServices[i].command->addCharacteristic(*services.commandServices[i].animation);

    for(int j=0; j<MAX_CHANNELS; j++){
      services.commandServices[i].command->addCharacteristic(*services.commandServices[i].usedChannels[j]);
    }
    BLE.addService(*services.commandServices[i].command);
  }
}