#include <Arduino.h>
#include <FlashStorage.h>
#include "defines.h"
#include "strips.h"

state currentState;
FlashStorage(flash_store, dynamicState);

int getNumUseablePositions(state* currentState, int channelIndex){
  if(currentState->dynamic.channels[channelIndex].height > 1){
    return ceil(currentState->dynamic.channels[channelIndex].width / 2);
  }else{
    return currentState->dynamic.channels[channelIndex].numLEDs;
  }
}

void saveState(dynamicState dState){
  flash_store.write(dState);//save current dynamic state to flash
}

state* getCurrentState(){
  return &currentState;
}

bool getInteriorSwitchState() {
  if(digitalRead(interiorTogglePin) == LOW){
    return true;
  }else{
    return true;//ALWAYS ON
  }
}

bool getAlternateCommandSwitchState() {
  if(digitalRead(alternateCommandTogglePin) == LOW){
    return false;//DISABLED
  }else{
    return false;
  }
}


bool getLeftTurnSwitchState() {
  if(digitalRead(leftTurnTogglePin) == LOW){
    return false;//DISABLED
  }else{
    return false;
  }
}

bool getRightTurnSwitchState() {
  if(digitalRead(rightTurnTogglePin) == LOW){
    return false;//DISABLED
  }else{
    return false;
  }
}

bool getBrakeSwitchState() {
  if(digitalRead(brakeTogglePin) == LOW){
    return false;//DISABLED
  }else{
    return false;
  }
}

void mapPositions(dynamicState *dState){
  Serial.println("Start mapping positions");
  memset (dState->mappedPositions, 0, sizeof(dState->mappedPositions));
  for(int i=0; i<NUM_ROWS; i++){
    for(int j=0; j<NUM_COLS; j++){
      int k=0;
      int value = i * NUM_COLS;
      value += j;  
      for(byte l=0; l < MAX_CHANNELS; l++){
        if(dState->channels[l].stripPosition == value){
          dState->mappedPositions[i][j][k] = l+1;//increment all strips id by 1 so we don't lose track of the 0 strip
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

bool isChannelActive(state* currentState, int channelIndex){
  if(getNumUseablePositions(currentState, channelIndex) > 0  && currentState->dynamic.channels[channelIndex].stripUsed == true){
    if(currentState->dynamic.channels[channelIndex].isInterior == false || (currentState->dynamic.channels[channelIndex].isInterior == true && currentState->temp.interiorOn == true)){
      return true;
    }
  }
  return false;
}

int getLongestChannelInLocation(state* currentState, int rowIndex, int colIndex){
  int highestLEDCount = 0;
  for(int j=0; j<MAX_CHANNELS; j++){
    int channelIndex = currentState->dynamic.mappedPositions[rowIndex][colIndex][j]-1;//indexes stored in mapped positions are increased by 1, so decreasing to use
    int numUseablePositions = getNumUseablePositions(currentState, channelIndex);
    if(numUseablePositions > highestLEDCount  && isChannelActive(currentState, channelIndex) == true){
        highestLEDCount = numUseablePositions;
    }
  }
  Serial.print("Longest channel for location Row: ");
  Serial.print(rowIndex);
  Serial.print(" Col: ");
  Serial.print(colIndex);
  Serial.print(" has ");
  Serial.print(highestLEDCount);
  Serial.println(" leds");
  return highestLEDCount;
}

int getLongestChannelInRow(state* currentState, int rowIndex){
  int highestLEDCount = 0;
  for(int i=0; i<NUM_COLS; i++){
    for(int j=0; j<MAX_CHANNELS; j++){
      int channelIndex = currentState->dynamic.mappedPositions[rowIndex][i][j]-1;//indexes stored in mapped positions are increased by 1, so decreasing to use
      int numUseablePositions = getNumUseablePositions(currentState, channelIndex);
      if(numUseablePositions > highestLEDCount  && isChannelActive(currentState, channelIndex) == true){
          highestLEDCount = numUseablePositions;
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


int getLastRow(state* currentState){
  int lastRow = 0;
  for(int i=0; i<NUM_ROWS; i++){
    if(getLongestChannelInRow(currentState, i) != 0){
      lastRow = i;
    }
  }
  return lastRow;
}

void setInitialChannelState(dynamicState *dState) {
  Serial.println("Setting initial channel state");//Front Right Rock Light
  dState->channels[0].isCentered = false;
  dState->channels[0].isInterior = false;
  dState->channels[0].stripUsed = true;
  dState->channels[0].directionFlipped = true;
  dState->channels[0].stripType = 1;
  dState->channels[0].stripOrder = 5;
  dState->channels[0].stripPosition = 2;
  dState->channels[0].numLEDs = 18;
  dState->channels[0].height = 1;
  dState->channels[0].width = 18;

  dState->channels[1].isCentered = false;//Right Interior Light
  dState->channels[1].isInterior = true;
  dState->channels[1].stripUsed = true;
  dState->channels[1].directionFlipped = true;
  dState->channels[1].stripType = 1;
  dState->channels[1].stripOrder = 5;
  dState->channels[1].stripPosition = 4;
  dState->channels[1].numLEDs = 18;
  dState->channels[1].height = 1;
  dState->channels[1].width = 18;

  dState->channels[2].isCentered = false;//Right Whip
  dState->channels[2].isInterior = false;
  dState->channels[2].stripUsed = true;
  dState->channels[2].directionFlipped = false;
  dState->channels[2].stripType = 1;
  dState->channels[2].stripOrder = 2;
  dState->channels[2].stripPosition = 8;
  dState->channels[2].numLEDs = 25;
  dState->channels[2].height = 1;
  dState->channels[2].width = 25;

  dState->channels[3].isCentered = false; //Right rear rock light
  dState->channels[3].isInterior = false;
  dState->channels[3].stripUsed = true;
  dState->channels[3].directionFlipped = true;
  dState->channels[3].stripType = 1;
  dState->channels[3].stripOrder = 5;
  dState->channels[3].stripPosition = 11;
  dState->channels[3].numLEDs = 18;
  dState->channels[3].height = 1;
  dState->channels[3].width = 18;

  dState->channels[4].isCentered = false;//Left Rear Rock Light
  dState->channels[4].isInterior = true;
  dState->channels[4].stripUsed = true;
  dState->channels[4].directionFlipped = true;
  dState->channels[4].stripType = 1;
  dState->channels[4].stripOrder = 5;
  dState->channels[4].stripPosition = 9;
  dState->channels[4].numLEDs = 18;
  dState->channels[4].height = 1;
  dState->channels[4].width = 18;

  dState->channels[5].isCentered = false;//Left Whip
  dState->channels[5].isInterior = false;
  dState->channels[5].stripUsed = true;
  dState->channels[5].directionFlipped = false;
  dState->channels[5].stripType = 1;
  dState->channels[5].stripOrder = 2;
  dState->channels[5].stripPosition = 6;
  dState->channels[5].numLEDs = 25;
  dState->channels[5].height = 1;
  dState->channels[5].width = 25;

  dState->channels[6].isCentered = false; //Left Interior
  dState->channels[6].isInterior = true;
  dState->channels[6].stripUsed = true;
  dState->channels[6].directionFlipped = true;
  dState->channels[6].stripType = 1;
  dState->channels[6].stripOrder = 5;
  dState->channels[6].stripPosition = 4;
  dState->channels[6].numLEDs = 18;
  dState->channels[6].height = 1;
  dState->channels[6].width = 18;

  dState->channels[7].isCentered = false;// Left Front Rock Light
  dState->channels[7].isInterior = false;
  dState->channels[7].stripUsed = true;
  dState->channels[7].directionFlipped = true;
  dState->channels[7].stripType = 1;
  dState->channels[7].stripOrder = 5;
  dState->channels[7].stripPosition = 0;
  dState->channels[7].numLEDs = 18;
  dState->channels[7].height = 1;
  dState->channels[7].width = 18;
}

commandState getAlternateCommandState(){
  commandState altCommand;
  altCommand.primaryRed = 255;
  altCommand.primaryGreen = 255;
  altCommand.primaryBlue = 255;
  altCommand.secondaryRed = 255;
  altCommand.secondaryGreen = 255;
  altCommand.secondaryBlue = 255;
  altCommand.animation = 1;
  altCommand.stepDelay = 999;
  return altCommand;
}

commandState getCommand(state* currentState){
  if(currentState->temp.alternateCommand == true){
    Serial.println("Using Alternate Command");
    return getAlternateCommandState();
  }else{
    return currentState->dynamic.command;
  }
}

void setInitialCommandState(dynamicState *dState){
  Serial.println("Setting initial command state");
  dState->command.primaryRed = 0;
  dState->command.primaryGreen = 255;
  dState->command.primaryBlue = 255;
  dState->command.secondaryRed = 255;
  dState->command.secondaryGreen = 0;
  dState->command.secondaryBlue = 255;
  dState->command.animation = 8;// Stack Alternate: 8
  dState->command.stepDelay = 10;
}

bool updateBrakeState(state* currentState){
  bool previousBrakeState = currentState->temp.brakeLight;
  currentState->temp.brakeLight = getBrakeSwitchState();
  if(previousBrakeState != currentState->temp.brakeLight){
    return true; //changed
  }else{
    return false;
  }
}

bool updateLeftTurnState(state* currentState){
  bool previousLeftTurnState = currentState->temp.leftTurnLight;
  currentState->temp.leftTurnLight = getLeftTurnSwitchState();
  if(previousLeftTurnState != currentState->temp.leftTurnLight){
    return true; //changed
  }else{
    return false;
  }
}

bool updateRightTurnState(state* currentState){
  bool previousRightTurnState = currentState->temp.rightTurnLight;
  currentState->temp.rightTurnLight = getRightTurnSwitchState();
  if(previousRightTurnState != currentState->temp.rightTurnLight){
    return true; //changed
  }else{
    return false;
  }
}

bool updateSignals(state* currentState){
  bool breakStateChanged = updateBrakeState(currentState);
  bool leftTurnStateChanged = updateLeftTurnState(currentState);
  bool rightTurnStateChanged = updateRightTurnState(currentState);

  if(currentState->temp.brakeLight == true || currentState->temp.leftTurnLight == true || currentState->temp.rightTurnLight == true){//one of the signals is on after change
    if(currentState->temp.lightsSaved == false){//a signal wasn't already on
      currentState->temp.lightsSaved = true;
      saveStripState(currentState);
    }
    return true;// signal is on
  }else{//no signals are on after change
    if(currentState->temp.lightsSaved == true){
      currentState->temp.lightsSaved = false;
      restoreStripState(currentState);
    }
    return false;//signal is off
  }
}

bool updateTempState(state *currentState){
  bool previousInteriorOnState = currentState->temp.interiorOn;
  currentState->temp.interiorOn = getInteriorSwitchState();
  
  bool previousAlternateCommandState = currentState->temp.alternateCommand;
  currentState->temp.alternateCommand = getAlternateCommandSwitchState();

  bool signalsChanged = updateSignals(currentState);
  if(previousAlternateCommandState != currentState->temp.alternateCommand || previousInteriorOnState != currentState->temp.interiorOn){
    return true; //state changed
  }else{
    return false; //state not changed
  }
}

void setInitialTempState(tempState *tState){
  Serial.println("Setting initial Temp state");
  pinMode(interiorTogglePin, INPUT_PULLUP);
  pinMode(alternateCommandTogglePin, INPUT_PULLUP);
  pinMode(leftTurnTogglePin, INPUT_PULLUP);
  pinMode(rightTurnTogglePin, INPUT_PULLUP);
  pinMode(brakeTogglePin, INPUT_PULLUP);
  tState->interiorOn = getInteriorSwitchState();
  tState->alternateCommand = getAlternateCommandSwitchState();
  tState->leftTurnLight = getLeftTurnSwitchState();
  tState->rightTurnLight = getRightTurnSwitchState();
  tState->brakeLight = getBrakeSwitchState();
  tState->lightsOn = true;
  tState->stepIndex = 0;
  tState->lastRowUsed = 0;
  tState->lastPosUsed = 0;
  tState->lightsSaved = false;
}

void setInitialDynamicState(dynamicState *dState){
  dState->hasUpdated = false;
  dState->validationKey = 1337;
  dynamicState savedDState = flash_store.read();
  if(savedDState.validationKey == dState->validationKey){
    Serial.println("Found saved dynamic state");
    *dState = savedDState;
  }else{
    Serial.println("Using default dynamic state");
    setInitialChannelState(dState);
    setInitialCommandState(dState);
  }
  mapPositions(dState);
}
