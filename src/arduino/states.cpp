#include <Arduino.h>
#include <FlashStorage.h>
#include "defines.h"

state currentState;
FlashStorage(flash_store, dynamicState);

void saveState(dynamicState dState){
  flash_store.write(dState);//save current dynamic state to flash
}

state* getCurrentState(){
  return &currentState;
}

bool getInteriorSwitchState() {
  if(digitalRead(interiorTogglePin) == HIGH){//might need to update to be LOW
    return true;
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
  if(currentState->dynamic.channels[channelIndex].numLEDs > 0  && currentState->dynamic.channels[channelIndex].stripUsed == true){
    if(currentState->dynamic.channels[channelIndex].isInterior == false || (currentState->dynamic.channels[channelIndex].isInterior == true && currentState->temp.interiorOn == true)){
      return true;
    }
  }
  return false;
}

int getLongestChannel(state* currentState, int rowIndex){
  int highestLEDCount = 0;
  for(int i=0; i<NUM_COLS; i++){
    for(int j=0; j<MAX_CHANNELS; j++){
      int channelIndex = currentState->dynamic.mappedPositions[rowIndex][i][j]-1;//indexes stored in mapped positions are increased by 1, so decreasing to use
      if(currentState->dynamic.channels[channelIndex].numLEDs > highestLEDCount  && isChannelActive(currentState, channelIndex) == true){
          highestLEDCount = currentState->dynamic.channels[channelIndex].numLEDs;
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
    if(getLongestChannel(currentState, i) != 0){
      lastRow = i;
    }
  }
  return lastRow;
}

void setInitialChannelState(dynamicState *dState) {
  Serial.println("Setting initial channel state");
  dState->channels[0].isCentered = false;
  dState->channels[0].isInterior = false;
  dState->channels[0].stripUsed = true;
  dState->channels[0].stripType = 1;
  dState->channels[0].stripOrder = 2;
  dState->channels[0].stripPosition = 9;
  dState->channels[0].numLEDs = 25;

  dState->channels[1].isCentered = false;
  dState->channels[1].isInterior = false;
  dState->channels[1].stripUsed = true;
  dState->channels[1].stripType = 1;
  dState->channels[1].stripOrder = 5;
  dState->channels[1].stripPosition = 6;
  dState->channels[1].numLEDs = 18;

  dState->channels[2].isCentered = false;
  dState->channels[2].isInterior = false;
  dState->channels[2].stripUsed = true;
  dState->channels[2].stripType = 1;
  dState->channels[2].stripOrder = 5;
  dState->channels[2].stripPosition = 0;
  dState->channels[2].numLEDs = 18;

  dState->channels[3].isCentered = false;
  dState->channels[3].isInterior = false;
  dState->channels[3].stripUsed = false;
  dState->channels[3].stripType = 1;
  dState->channels[3].stripOrder = 5;
  dState->channels[3].stripPosition = 4;
  dState->channels[3].numLEDs = 18;

  dState->channels[4].isCentered = false;
  dState->channels[4].isInterior = true;
  dState->channels[4].stripUsed = false;
  dState->channels[4].stripType = 1;
  dState->channels[4].stripOrder = 5;
  dState->channels[4].stripPosition = 4;
  dState->channels[4].numLEDs = 18;

  dState->channels[5].isCentered = false;
  dState->channels[5].isInterior = false;
  dState->channels[5].stripUsed = true;
  dState->channels[5].stripType = 1;
  dState->channels[5].stripOrder = 5;
  dState->channels[5].stripPosition = 2;
  dState->channels[5].numLEDs = 18;

  dState->channels[6].isCentered = false;
  dState->channels[6].isInterior = false;
  dState->channels[6].stripUsed = true;
  dState->channels[6].stripType = 1;
  dState->channels[6].stripOrder = 5;
  dState->channels[6].stripPosition = 8;
  dState->channels[6].numLEDs = 18;

  dState->channels[7].isCentered = false;
  dState->channels[7].isInterior = false;
  dState->channels[7].stripUsed = true;
  dState->channels[7].stripType = 1;
  dState->channels[7].stripOrder = 2;
  dState->channels[7].stripPosition = 11;
  dState->channels[7].numLEDs = 25;
}

void setInitialCommandState(dynamicState *dState){
  Serial.println("Setting initial command state");
  dState->command.primaryRed = 0;
  dState->command.primaryGreen = 255;
  dState->command.primaryBlue = 255;
  dState->command.secondaryRed = 255;
  dState->command.secondaryGreen = 0;
  dState->command.secondaryBlue = 255;
  dState->command.animation = 8;
  dState->command.stepDelay = 100;
}

void setInitialTempState(tempState *tState){
  Serial.println("Setting initial Temp state");
  tState->interiorOn = getInteriorSwitchState();
  tState->lightsOn = true;
  tState->stepIndex = 0;
  tState->lastRowUsed = 0;
  tState->lastLEDUsed = 0;
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
