#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "defines.h"
#include "states.h"

void clearStrips(staticState* sState){
  for(int i=0; i<MAX_CHANNELS; i++){
    sState->strips[i]->clear();
  }
}

void fillStrips(state* currentState, uint32_t color){
  for(int i=0; i<MAX_CHANNELS; i++){
    if(isChannelActive(currentState, i) == true){
      for(int j=0; j<currentState->dynamic.channels[i].numLEDs; j++){
        currentState->constant.strips[i]->setPixelColor(j, color);
      }
      currentState->constant.strips[i]->show();
    }
  }
}

void showAllStrips(state* currentState){
  for(int i=0; i<MAX_CHANNELS; i++){
    if(isChannelActive(currentState, i) == true){
      currentState->constant.strips[i]->show();
    }
  }
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

void setStrip(state* currentState, int i){
    delete currentState->constant.strips[i];
    currentState->constant.strips[i] = getStripDefinition(i, currentState->dynamic.channels[i].stripType, currentState->dynamic.channels[i].stripOrder, currentState->dynamic.channels[i].numLEDs);
    currentState->constant.strips[i]->begin();
    pinMode(outputPins[i], OUTPUT);
    digitalWrite(outputPins[i], LOW);
    currentState->constant.strips[i]->clear();
}

void setInitialStrips(state* currentState){
  for(int i=0; i<MAX_CHANNELS; i++){
    setStrip(currentState, i);
  }
}

bool setStripColorAtPosition(state* currentState, int row, int col, int led, uint32_t color){
  for(int channel=0; channel < MAX_CHANNELS; channel++){ //loop over all channels to see if they are in this row and column
    int channelIndex = currentState->dynamic.mappedPositions[row][col][channel] - 1; //we added 1 to the index so we didn't lose it when storing, removing it here.  stores 1 or 0 if the channel is in this position
    
    if(channelIndex >= 0){//channel is in specified position
      if(isChannelActive(currentState, channelIndex) == true){//this channel is active and accepts updates
        if(currentState->dynamic.channels[channelIndex].isInterior == false || (currentState->dynamic.channels[channelIndex].isInterior == true && getInteriorSwitchState() == true)){ //channel is not interior, or if it is the interior switch is on
           if(currentState->dynamic.channels[channelIndex].isCentered == true){//updates on this channel start in the center and work towards the end
            //logic to start at the center of the strip and work outwards
            //will need to handle odd and even strips where the middle point may be 2 leds togeather
            //also need to account for the last led to be all the side leds of a square (for lightbars with led rings around them)
          }else{//updates on this channel start at the beginning and work their way to the other end
            if(currentState->dynamic.channels[channelIndex].numLEDs >= led){ //strip has an led at that position to update
              currentState->constant.strips[channelIndex]->setPixelColor(led, color);
          }
        }
       }
      }
    }
  }
}
