#include <FlashStorage.h>
#include <ArduinoBLE.h>
#include <Adafruit_NeoPixel.h>

#define MAX_LEDS_PER_CHANNEL 300
#define MAX_CHANNELS 8
#define NUM_ROWS 4
#define NUM_COLS 3
#define UUID_LENGTH 37

const int interiorTogglePin = 10;
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
  bool interiorOn;
  bool lightsOn;
  word stepIndex;
  word lastLEDUsed;
  int lastRowUsed;
};

dynamicState dState;
staticState sState;
tempState tState;

FlashStorage(flash_store, dynamicState);

void setup() {
  pinMode(interiorTogglePin, INPUT);
  Serial.begin(57600);
  //while(!Serial);//temporary stops running until serial connection is made
  Serial.println("Starting BLE");
  while (!BLE.begin()) {
    Serial.println("Waiting for BLE to start");
    delay(1);
  }
  BLE.setDeviceName("SquadGlowController");
  BLE.setLocalName("SquadGlowController");
  setupTempState();
  setInitialDynamicState();
  describeDynamicState();
  setupService();
  BLE.setAdvertisedService(*sState.data);
  BLE.advertise();//I think only 1 is advertised at a time
  setInitialCharacteristicValues();
  setInitialStrips();
  Serial.println("Ready!");
}

void loop() {
  // put your main code here, to run repeatedly:
  delayAndPoll(0);
  runCommand();
}

void runCommand() {
  switch(dState.command.animation){
    case 0:
      staticCommand();
      break;
    case 1:
      blinkCommand();
      break;
    case 2:
      pulseCommand();
      break;
    case 3:
      waveCommand();
      break;
    case 4:
      rollCommand(false);
      break;
    case 5:
      rollCommand(true);
      break;
    case 6:
      stackCommand(false);
      break;
    case 7:
      stackCommand(true);
      break;
    default:
      staticCommand();
  }
}

void staticCommand(){
  Serial.println("Starting static command");
  uint32_t color = sState.strips[0]->Color(dState.command.primaryRed, dState.command.primaryGreen, dState.command.primaryBlue);
  fillStrips(color);
  delayAndPoll(dState.command.stepDelay);
  Serial.println("Finished static command");
}

void blinkCommand(){
  Serial.println("Starting blink command");
  uint32_t primaryColor = sState.strips[0]->Color(dState.command.primaryRed, dState.command.primaryGreen, dState.command.primaryBlue);
  uint32_t secondaryColor = sState.strips[0]->Color(dState.command.secondaryRed, dState.command.secondaryGreen, dState.command.secondaryBlue);

  for(int i=0; i<MAX_CHANNELS; i++){
    if(isChannelActive(i) == true){
      for(int j=0; j<dState.channels[i].numLEDs; j++){
        sState.strips[i]->setPixelColor(j, primaryColor);
      }
      sState.strips[i]->show();
    }
  }
  
  if(delayAndPoll(dState.command.stepDelay) == false){//returns true if there was an update, stop command if there was to start working on new state
    for(int i=0; i<MAX_CHANNELS; i++){
      if(isChannelActive(i) == true){
        for(int j=0; j<dState.channels[i].numLEDs; j++){
          sState.strips[i]->setPixelColor(j, secondaryColor);
        }
        sState.strips[i]->show();
      }
    }
  }
  delayAndPoll(dState.command.stepDelay);
  Serial.println("Finished blink command");
}

void pulseCommand(){
  Serial.println("Starting pulse command");
  clearStrips();
  
  bool atTarget = false;
  int curRed = 0;
  int curGreen = 0;
  int curBlue = 0;

  while(!atTarget){//up Primary
    if(curRed < dState.command.primaryRed){
      curRed++;
    }
    if(curGreen < dState.command.primaryGreen){
      curGreen++;
    }
    if(curBlue < dState.command.primaryBlue){
      curBlue++;
    }
    if(curRed == dState.command.primaryRed && curGreen == dState.command.primaryGreen && curBlue == dState.command.primaryBlue){
      atTarget = true;
    }
    uint32_t color = sState.strips[0]->Color(curRed, curGreen, curBlue);
    for(int i=0; i<MAX_CHANNELS; i++){
      if(isChannelActive(i) == true){
        for(int j=0; j<dState.channels[i].numLEDs; j++){
          sState.strips[i]->setPixelColor(j, color);
        }
        sState.strips[i]->show();
      }
    }
    delayAndPoll(dState.command.stepDelay);
  }

  atTarget = false;
  while(!atTarget){//down primary
    if(curRed > 0){
      curRed--;
    }
    if(curGreen > 0){
      curGreen--;
    }
    if(curBlue > 0){
      curBlue--;
    }
    if(curRed == 0 && curGreen == 0 && curBlue == 0){
      atTarget = true;
    }
    uint32_t color = sState.strips[0]->Color(curRed, curGreen, curBlue);
    for(int i=0; i<MAX_CHANNELS; i++){
      if(isChannelActive(i) == true){
        for(int j=0; j<dState.channels[i].numLEDs; j++){
          sState.strips[i]->setPixelColor(j, color);
        }
        sState.strips[i]->show();
      }
    }
    delayAndPoll(dState.command.stepDelay);
  }
    atTarget = false;
    while(!atTarget){//up Secondary
    if(curRed < dState.command.secondaryRed){
      curRed++;
    }
    if(curGreen < dState.command.secondaryGreen){
      curGreen++;
    }
    if(curBlue < dState.command.secondaryBlue){
      curBlue++;
    }
    if(curRed == dState.command.secondaryRed && curGreen == dState.command.secondaryGreen && curBlue == dState.command.secondaryBlue){
      atTarget = true;
    }
    uint32_t color = sState.strips[0]->Color(curRed, curGreen, curBlue);
    for(int i=0; i<MAX_CHANNELS; i++){
      if(isChannelActive(i) == true){
        for(int j=0; j<dState.channels[i].numLEDs; j++){
          sState.strips[i]->setPixelColor(j, color);
        }
        sState.strips[i]->show();
      }
    }
    delayAndPoll(dState.command.stepDelay);
  }

  atTarget = false;
  while(!atTarget){//down primary
    if(curRed > 0){
      curRed--;
    }
    if(curGreen > 0){
      curGreen--;
    }
    if(curBlue > 0){
      curBlue--;
    }
    if(curRed == 0 && curGreen == 0 && curBlue == 0){
      atTarget = true;
    }
    uint32_t color = sState.strips[0]->Color(curRed, curGreen, curBlue);
    for(int i=0; i<MAX_CHANNELS; i++){
      if(isChannelActive(i) == true){
        for(int j=0; j<dState.channels[i].numLEDs; j++){
          sState.strips[i]->setPixelColor(j, color);
        }
        sState.strips[i]->show();
      }
    }
    delayAndPoll(dState.command.stepDelay);
  }
  Serial.println("Finished pulse command");
}

void waveCommand(){
  Serial.println("Started wave command");
  uint32_t primaryColor = sState.strips[0]->Color(dState.command.primaryRed, dState.command.primaryGreen, dState.command.primaryBlue);
  uint32_t secondaryColor = sState.strips[0]->Color(dState.command.secondaryRed, dState.command.secondaryGreen, dState.command.secondaryBlue);

  fillStrips(primaryColor);

  for(int row=0; row<NUM_ROWS; row++){
    Serial.println("Switched rows, showing all strips");
    showAllStrips();
    int longestChannel = getLongestChannel(row);
    for(int led=0; led<longestChannel; led++){
      bool changed = false;
      for(int col=0; col<NUM_COLS; col++){
        for(int channel=0; channel < MAX_CHANNELS; channel++){
          int channelIndex = dState.mappedPositions[row][col][channel] - 1; //we added 1 to the index so we didn't lose it when storing, removing it here
          if(channelIndex >= 0 && isChannelActive(channelIndex) == true){//channelIndex will be -1 if there was no value stored there
              changed = true;
             if(dState.channels[channelIndex].numLEDs >= led){ //not at end of this strip
                sState.strips[channelIndex]->setPixelColor(led, secondaryColor);
                if(led != 0){
                  sState.strips[channelIndex]->setPixelColor(led-1, primaryColor); //set previous led back to primary color
                }
                Serial.print("Setting channel");
                Serial.print(channelIndex);
                Serial.print("'s ");
                Serial.print(led);
                Serial.println(" led to secondary color, and previous pixel to primary color");
             }else{
                sState.strips[channelIndex]->setPixelColor(dState.channels[channelIndex].numLEDs-1, primaryColor); //set last pixel in the strip back to primary Color
                Serial.print("Setting channel ");
                Serial.print(channelIndex);
                Serial.println("'s last pixel back to primaryColor");
             }
             sState.strips[channelIndex]->show();
             if(led == longestChannel-1){
              sState.strips[channelIndex]->setPixelColor(led, primaryColor);//go ahead and set the color back to primary on longest strip, will update all strips to set so we don't have to remember which strips
              Serial.print("Setting channel");
              Serial.print(channelIndex);
              Serial.println("'s last pixel back to primaryColor before switching rows");
             }
          }
        }
      }
      if(changed){
        delayAndPoll(dState.command.stepDelay);
      }
    }
  }
  Serial.println("Finished wave command");
}

void rollCommand(bool alternate){
  Serial.println("Started roll command");
  uint32_t primaryColor = sState.strips[0]->Color(dState.command.primaryRed, dState.command.primaryGreen, dState.command.primaryBlue);
  uint32_t secondaryColor = sState.strips[0]->Color(dState.command.secondaryRed, dState.command.secondaryGreen, dState.command.secondaryBlue);

  if(alternate){
    if(tState.stepIndex == 1){
      primaryColor = sState.strips[0]->Color(dState.command.secondaryRed, dState.command.secondaryGreen, dState.command.secondaryBlue);
      secondaryColor = sState.strips[0]->Color(dState.command.primaryRed, dState.command.primaryGreen, dState.command.primaryBlue);
      tState.stepIndex = 0;
    }else{
      tState.stepIndex = 1;
    }
  }

  fillStrips(primaryColor);

  for(int row=0; row<NUM_ROWS; row++){
    Serial.println("Switched rows, showing all strips");
    showAllStrips();
    int longestChannel = getLongestChannel(row);
    for(int led=0; led<longestChannel; led++){
      bool changed = false;
      for(int col=0; col<NUM_COLS; col++){
        for(int channel=0; channel < MAX_CHANNELS; channel++){
          int channelIndex = dState.mappedPositions[row][col][channel] - 1; //we added 1 to the index so we didn't lose it when storing, removing it here
          if(channelIndex >= 0 && isChannelActive(channelIndex) == true){//channelIndex will be -1 if there was no value stored there
              changed = true;
             if(dState.channels[channelIndex].numLEDs >= led){ //not at end of this strip
                sState.strips[channelIndex]->setPixelColor(led, secondaryColor);
                Serial.print("Setting channel");
                Serial.print(channelIndex);
                Serial.print("'s ");
                Serial.print(led);
                Serial.println(" led to secondary color");
             }
             sState.strips[channelIndex]->show();
          }
        }
      }
      if(changed){
        delayAndPoll(dState.command.stepDelay);
      }
    }
  }
  Serial.println("Finished roll command");
}

void stackCommand(bool alternate){
  Serial.println("Started stack command");
  uint32_t primaryColor = 0;
  uint32_t secondaryColor = 0;

  if(alternate){//will alterante which color is stacked after a full cycl
    if( tState.lastRowUsed == 0 && tState.lastLEDUsed == 0){//completed a stack
      Serial.println("Swapping Primary Color");
      if(tState.stepIndex == 1){
        Serial.println("Switching to alternate colors");
        tState.stepIndex = 0;
      }else{
        Serial.println("Switching to standard colors");
        tState.stepIndex = 1;
      } 
    }
  }

  if(tState.stepIndex == 0 || alternate == false){
    Serial.println("Using alternate colors");
    primaryColor = sState.strips[0]->Color(dState.command.secondaryRed, dState.command.secondaryGreen, dState.command.secondaryBlue);
    secondaryColor = sState.strips[0]->Color(dState.command.primaryRed, dState.command.primaryGreen, dState.command.primaryBlue);
  }else{
    Serial.println("Using standard colors");
    primaryColor = sState.strips[0]->Color(dState.command.primaryRed, dState.command.primaryGreen, dState.command.primaryBlue);
    secondaryColor = sState.strips[0]->Color(dState.command.secondaryRed, dState.command.secondaryGreen, dState.command.secondaryBlue);
  }
  
  if( tState.lastRowUsed == 0 && tState.lastLEDUsed == 0){ //reset to last led in last row
    fillStrips(primaryColor);
    Serial.println("Getting last row and channel");
    tState.lastRowUsed = getLastRow();
    tState.lastLEDUsed = getLongestChannel(tState.lastRowUsed);
  }else{
    if(tState.lastLEDUsed == 0){
      Serial.print("Row ");
      Serial.print(tState.lastRowUsed);
      Serial.println(" complete, getting next row");
      
      for(int i=tState.lastRowUsed-1; i>= 0; i--){
        int nextChannel = getLongestChannel(i);
        if(nextChannel > 0){
          tState.lastRowUsed = i;
          tState.lastLEDUsed = nextChannel;
          Serial.print("Next row is ");
          Serial.println(tState.lastRowUsed);
          break;
        }
      }
    }else{
      Serial.println("Decrementing LED used");
      tState.lastLEDUsed--;
    }
  }
  
  Serial.print("Stacking Row ");
  Serial.println(tState.lastRowUsed);
  Serial.print("Stacking LED ");
  Serial.println(tState.lastLEDUsed-1);
  for(int row=0; row<NUM_ROWS; row++){
    if(row > tState.lastRowUsed){
      break;//don't need to update anymore
    }
    Serial.print("Updating row ");
    Serial.println(row);
    showAllStrips();
    int longestChannel = getLongestChannel(row);
    for(int led=0; led<longestChannel; led++){
      bool changed = false;//only want to delay if there was a change
      for(int col=0; col<NUM_COLS; col++){
        for(int channel=0; channel < MAX_CHANNELS; channel++){
          int channelIndex = dState.mappedPositions[row][col][channel] - 1; //we added 1 to the index so we didn't lose it when storing, removing it here.  stores 1 or 0 if the channel is in this position
          if(channelIndex >= 0 && isChannelActive(channelIndex) == true){//channelIndex will be -1 if there was no value stored there
             if(dState.channels[channelIndex].numLEDs >= led){ //not at end of this strip
                if(row <= tState.lastRowUsed){ //this row hasn't completed a stack yet
                  if(row == tState.lastRowUsed && led >= tState.lastLEDUsed){//this led is already stacked so do nothing
                    Serial.print("Channel ");
                    Serial.print(channelIndex);
                    Serial.print("'s ");
                    Serial.print(led);
                    Serial.println(" led is stacked");
                  }else{//this led hasn't been stacked yet so apply updates
                    changed = true;
                    sState.strips[channelIndex]->setPixelColor(led, secondaryColor); //set current pixel to secondary color
                    if(led != 0){
                      sState.strips[channelIndex]->setPixelColor(led-1, primaryColor); //set previous led back to primary color
                    }
                    Serial.print("Setting channel ");
                    Serial.print(channelIndex);
                    Serial.print("'s ");
                    Serial.print(led);
                    Serial.println(" led to secondary color, and previous pixel to primary color");
                  }
                }
             }else{//end of the strip if its not the longest strip in the row
               if(row < tState.lastRowUsed){//if this row is below the last row used              
                  sState.strips[channelIndex]->setPixelColor(dState.channels[channelIndex].numLEDs-1, primaryColor); //set last pixel in the strip back to primary Color, showing all strips at the start of next loops well display it and complete the animation
                  Serial.print("Setting channel ");
                  Serial.print(channelIndex);
                  Serial.println("'s last pixel back to primaryColor, not the longest channel in the row");
               }
             }
             sState.strips[channelIndex]->show();
             if(led == longestChannel-1){  //on last led in the longest strip and will be switching rows next
              if(row != tState.lastRowUsed){//stacking this led      
                changed = true;
                sState.strips[channelIndex]->setPixelColor(led, primaryColor);//go ahead and set the color back to primary on longest strip, will update all strips to set so we don't have to remember which strips
                Serial.print("Setting channel ");
                Serial.print(channelIndex);
                Serial.print("'s last pixel(");
                Serial.print(led);
                Serial.println(") back to primaryColor before switching rows");
              }
             }
          }
        }
      }
      if(changed){ //if a pixel changes color
        delayAndPoll(dState.command.stepDelay);//delay should happen before next pixel changes
      }
    }
  }
  Serial.println("Finished stack command");
}

bool delayAndPoll(int sleep){
  if(sleep <= 0){
    BLE.poll();
    while(BLE.connected()){
      Serial.println("Paused while central connected");
      BLE.poll();
    }
    if(dState.hasUpdated == true){
      Serial.println("Has found update");
      clearStrips();
      tState.stepIndex = 0;
      dState.hasUpdated = false;
      return true;
    }
  }else{
    bool hasUpdated = false;
    for(int i=0; i<sleep; i++){
      BLE.poll();
      delay(1);
      while(BLE.connected()){
        Serial.println("Paused while central connected");
        BLE.poll();
      }
      if(dState.hasUpdated == true){
        Serial.println("Has found update");
        clearStrips();
        tState.stepIndex = 0;
        dState.hasUpdated = false;
        hasUpdated = true;
      }
    }
    return hasUpdated;
  }
}

void updateInteriorSwithState() {
  tState.interiorOn = getInteriorSwitchState();
}

bool getInteriorSwitchState() {
  if(digitalRead(interiorTogglePin) == HIGH){//might need to update to be LOW
    return true;
  }else{
    return false;
  }
}

void setupTempState(){
  updateInteriorSwithState();
  tState.lightsOn = true;
  tState.stepIndex = 0;
  tState.lastRowUsed = 0;
  tState.lastLEDUsed =0;
}

void setupService(){
  Serial.println("Start building service");
  sState.data = new BLEService(serviceUUID);
  
  sState.save = new BLEBoolCharacteristic(characteristicUUIDs[0], BLERead | BLEWrite);
  sState.save->setEventHandler(BLEWritten, saveDynamicState);
  sState.data->addCharacteristic(*sState.save);
  
  sState.on = new BLEBoolCharacteristic(characteristicUUIDs[1], BLERead | BLEWrite);
  sState.on->setEventHandler(BLEWritten, toggleLights);
  sState.data->addCharacteristic(*sState.on);

  sState.command = new BLECharacteristic(characteristicUUIDs[2], BLERead | BLEWrite, sizeof(commandState), 512);
  sState.command->setEventHandler(BLEWritten, updateCommand);
  sState.data->addCharacteristic(*sState.command);
  
  sState.bulkRead = new BLECharacteristic(characteristicUUIDs[3], BLERead, sizeof(bulkRead), 512);
  sState.data->addCharacteristic(*sState.bulkRead);

  for(int i=0; i<MAX_CHANNELS; i++){
    sState.channels[i] = new BLECharacteristic(characteristicUUIDs[4+i], BLERead | BLEWrite, sizeof(channelState), 100); //index +4 since channels start at 4th
    sState.channels[i]->setEventHandler(BLEWritten, updateChannel);
    sState.data->addCharacteristic(*sState.channels[i]);
  }
  BLE.addService(*sState.data);
  Serial.println("Finish building service");
}

void saveDynamicState(BLEDevice central, BLECharacteristic characteristic){
  if( sState.save->value() == true){
    Serial.println("Started saving dynamic state");
    flash_store.write(dState);//save current dynamic state to flash
    sState.save->writeValue(false); //set back to false when finished to signify completion
    Serial.println("Finished saving dynamic state");
  }
} 

void toggleLights(BLEDevice central, BLECharacteristic characteristic){
  Serial.println("New light stats value found");
  if(sState.on->value() == true){
    Serial.println("Turning lights on");
    tState.lightsOn = true;
  }else{
    Serial.println("Turning lights off");
    tState.lightsOn = false;
  }
  updateBulkReadChar();
}

void updateCommand(BLEDevice central, BLECharacteristic characteristic){
  Serial.println("Recieved new command state");
  commandUnion converter;
  sState.command->readValue(converter.serializedArray,sizeof(commandState));
  dState.command = converter.packet;
  dState.hasUpdated = true;
  Serial.println("Applied new command state");
  describeCommandState();
  updateBulkReadChar();
}

void updateChannel(BLEDevice central, BLECharacteristic characteristic){
  Serial.println("Recieved new channel state");
  int channelIndex = 0;
  for(int i=0; i<MAX_CHANNELS; i++){//determine which channel was updated
    if(characteristic.uuid() == characteristicUUIDs[i+3]){
      channelIndex = i;
      Serial.println("Matching channel UUID found for update");
    }
  }
  
  channelUnion converter;
  sState.channels[channelIndex]->readValue(converter.serializedArray, sizeof(channelState));
  dState.channels[channelIndex] = converter.packet; 
  setStrip(channelIndex);
  dState.hasUpdated = true;
  mapPositions();
  Serial.println("Applied new channel state");
  describeChannelState(channelIndex);
  updateBulkReadChar();
}

Adafruit_NeoPixel* getStripDefinition(int channelIndex, int type, int order, int numLEDs) {
  Serial.println("Start getting strip definition");
  int pixelOrder = 0;
  int pixelType = 0;

  switch (type) {
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

  switch (order) {
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
  return new Adafruit_NeoPixel(numLEDs, outputPins[channelIndex], pixelFormat);
}

void describeDynamicState(){
  describeCommandState();
  for(int i=0; i<MAX_CHANNELS; i++){
    describeChannelState(i);
  }
}

void describeCommandState(){
  Serial.println("Start Describing Command State - ");
  
  Serial.print("Primary Red: ");
  Serial.println(dState.command.primaryRed);

  Serial.print("Primary Green: ");
  Serial.println(dState.command.primaryGreen);

  Serial.print("Primary Blue: ");
  Serial.println(dState.command.primaryBlue);

  Serial.print("Secondary Red: ");
  Serial.println(dState.command.secondaryRed);

  Serial.print("Secondary Green: ");
  Serial.println(dState.command.secondaryGreen);

  Serial.print("Secondary Blue: ");
  Serial.println(dState.command.secondaryBlue);

  Serial.print("Animation: ");
  Serial.println(dState.command.animation);

  Serial.print("Step Delay: ");
  Serial.println(dState.command.stepDelay);

  Serial.println("Finish Describing Command State");
}

void describeChannelState(int channelIndex){
  Serial.print("Start Describing Channel ");
  Serial.print(channelIndex);
  Serial.println(" - ");

  Serial.print("Strip Is Centered: ");
  Serial.println(dState.channels[channelIndex].isCentered);

  Serial.print("Strip Is Interior: ");
  Serial.println(dState.channels[channelIndex].isInterior);

  Serial.print("Strip Used: ");
  Serial.println(dState.channels[channelIndex].stripUsed);

  Serial.print("Strip Type: ");
  Serial.println(dState.channels[channelIndex].stripType);

  Serial.print("Strip Order: ");
  Serial.println(dState.channels[channelIndex].stripOrder);

  Serial.print("Strip Position: ");
  Serial.println(dState.channels[channelIndex].stripPosition);

  Serial.print("Number LEDs: ");
  Serial.println(dState.channels[channelIndex].numLEDs);
}

void setInitialCharacteristicValues(){
  Serial.println("Start setting initial service values");

  sState.on->writeValue(tState.lightsOn);
  
  commandUnion commandConverter;
  commandConverter.packet = dState.command;
  sState.command->writeValue(commandConverter.serializedArray, sizeof(commandConverter.serializedArray));

  channelUnion channelConverter;
  for(int i=0; i<MAX_CHANNELS; i++){
    channelConverter.packet = dState.channels[i];
    sState.channels[i]->writeValue(channelConverter.serializedArray, sizeof(channelConverter.serializedArray));
  }
  updateBulkReadChar();
  
  Serial.println("Finish setting initial service values");
}

void updateBulkReadChar(){
  bulkReadUnion bulkReadConverter;
  bulkReadConverter.packet.on = tState.lightsOn;
  bulkReadConverter.packet.command = dState.command;
  memcpy(bulkReadConverter.packet.channels, dState.channels, sizeof(dState.channels));
  //bulkReadConverter.packet.channels = dState.channels;
  sState.bulkRead->writeValue(bulkReadConverter.serializedArray, sizeof(bulkReadConverter.serializedArray));
}

void setInitialStrips(){
  for(int i=0; i<MAX_CHANNELS; i++){
    setStrip(i);
  }
}

void clearStrips(){
  for(int i=0; i<MAX_CHANNELS; i++){
    sState.strips[i]->clear();
  }
}

void setStrip(int i){
    delete sState.strips[i];
    sState.strips[i] = getStripDefinition(i, dState.channels[i].stripType, dState.channels[i].stripOrder, dState.channels[i].numLEDs);
    sState.strips[i]->begin();
    pinMode(outputPins[i], OUTPUT);
    digitalWrite(outputPins[i], LOW);
    sState.strips[i]->clear();
}

void mapPositions(){
  Serial.println("Start mapping positions");
  memset (dState.mappedPositions, 0, sizeof(dState.mappedPositions));
  for(int i=0; i<NUM_ROWS; i++){
    for(int j=0; j<NUM_COLS; j++){
      int k=0;
      int value = i * NUM_COLS;
      value += j;  
      /*Serial.print("Row: ");
      Serial.print(i);
      Serial.print(" Col: ");
      Serial.print(j);
      Serial.print(" generates value: ");
      Serial.println(value);*/
      for(byte l=0; l < MAX_CHANNELS; l++){
        if(dState.channels[l].stripPosition == value){
          dState.mappedPositions[i][j][k] = l+1;//increment all strips id by 1 so we don't lose track of the 0 strip
          k++; //used to keep track of multiple strips in the same positions
          Serial.print("Strip ");
          Serial.print(l);
          Serial.print(" is in row ");
          Serial.print(i);
          Serial.print(" and column ");
          Serial.println(j);
        }
      }
    }
  }
  Serial.println("Finnished mapping positions");
}

int getLastRow(){
  int lastRow = 0;
  for(int i=0; i<NUM_ROWS; i++){
    if(getLongestChannel(i) != 0){
      lastRow = i;
    }
  }
  return lastRow;
}

int getLongestChannel(int rowIndex){
  int highestLEDCount = 0;
  for(int i=0; i<NUM_COLS; i++){
    for(int j=0; j<MAX_CHANNELS; j++){
      int channelIndex = dState.mappedPositions[rowIndex][i][j]-1;//indexes stored in mapped positions are increased by 1, so decreasing to use
      if(dState.channels[channelIndex].numLEDs > highestLEDCount  && isChannelActive(channelIndex) == true){
          highestLEDCount = dState.channels[channelIndex].numLEDs;
      }
    }
  }
  Serial.print("Longest channel for row ");
  Serial.print(rowIndex);
  Serial.print(" has ");
  Serial.print(highestLEDCount);
  Serial.println(" leds");
  return highestLEDCount;
}

int fillStrips(uint32_t color){
  for(int i=0; i<MAX_CHANNELS; i++){
    if(isChannelActive(i) == true){
      for(int j=0; j<dState.channels[i].numLEDs; j++){
        sState.strips[i]->setPixelColor(j, color);
      }
      sState.strips[i]->show();
    }
  }
}

int showAllStrips(){
  for(int i=0; i<MAX_CHANNELS; i++){
    if(isChannelActive(i) == true){
      sState.strips[i]->show();
    }
  }
}

bool isChannelActive(int channelIndex){
  if(dState.channels[channelIndex].numLEDs > 0  && dState.channels[channelIndex].stripUsed == true){
    if(dState.channels[channelIndex].isInterior == false || (dState.channels[channelIndex].isInterior == true && tState.interiorOn == true)){
      return true;
    }
  }
  return false;
}

void setInitialDynamicState(){
  dState.hasUpdated = false;
  dState.validationKey = 1337;
  dynamicState savedDState = flash_store.read();
  if(savedDState.validationKey == dState.validationKey){
    Serial.println("Found saved dynamic state");
    dState = savedDState;
  }else{
    Serial.println("Using default dynamic state");
    setInitialChannelState();
    setInitialCommandState();
  }
  mapPositions();
}

void setInitialChannelState() {
  dState.channels[0].isCentered = false;
  dState.channels[0].isInterior = false;
  dState.channels[0].stripUsed = true;
  dState.channels[0].stripType = 1;
  dState.channels[0].stripOrder = 2;
  dState.channels[0].stripPosition = 9;
  dState.channels[0].numLEDs = 25;

  dState.channels[1].isCentered = false;
  dState.channels[1].isInterior = false;
  dState.channels[1].stripUsed = true;
  dState.channels[1].stripType = 1;
  dState.channels[1].stripOrder = 5;
  dState.channels[1].stripPosition = 6;
  dState.channels[1].numLEDs = 18;

  dState.channels[2].isCentered = false;
  dState.channels[2].isInterior = false;
  dState.channels[2].stripUsed = true;
  dState.channels[2].stripType = 1;
  dState.channels[2].stripOrder = 5;
  dState.channels[2].stripPosition = 0;
  dState.channels[2].numLEDs = 18;

  dState.channels[3].isCentered = false;
  dState.channels[3].isInterior = false;
  dState.channels[3].stripUsed = false;
  dState.channels[3].stripType = 1;
  dState.channels[3].stripOrder = 5;
  dState.channels[3].stripPosition = 4;
  dState.channels[3].numLEDs = 18;

  dState.channels[4].isCentered = false;
  dState.channels[4].isInterior = true;
  dState.channels[4].stripUsed = false;
  dState.channels[4].stripType = 1;
  dState.channels[4].stripOrder = 5;
  dState.channels[4].stripPosition = 4;
  dState.channels[4].numLEDs = 18;

  dState.channels[5].isCentered = false;
  dState.channels[5].isInterior = false;
  dState.channels[5].stripUsed = true;
  dState.channels[5].stripType = 1;
  dState.channels[5].stripOrder = 5;
  dState.channels[5].stripPosition = 2;
  dState.channels[5].numLEDs = 18;

  dState.channels[6].isCentered = false;
  dState.channels[6].isInterior = false;
  dState.channels[6].stripUsed = true;
  dState.channels[6].stripType = 1;
  dState.channels[6].stripOrder = 5;
  dState.channels[6].stripPosition = 8;
  dState.channels[6].numLEDs = 18;

  dState.channels[7].isCentered = false;
  dState.channels[7].isInterior = false;
  dState.channels[7].stripUsed = true;
  dState.channels[7].stripType = 1;
  dState.channels[7].stripOrder = 2;
  dState.channels[7].stripPosition = 11;
  dState.channels[7].numLEDs = 25;
  Serial.println("Setting initial channel state");
}

void setInitialCommandState(){
  Serial.println("Setting initial command state");
  dState.command.primaryRed = 0;
  dState.command.primaryGreen = 255;
  dState.command.primaryBlue = 255;
  dState.command.secondaryRed = 255;
  dState.command.secondaryGreen = 0;
  dState.command.secondaryBlue = 255;
  dState.command.animation = 7;
  dState.command.stepDelay = 100;
}
