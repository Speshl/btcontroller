#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "defines.h"
#include "states.h"
#include <math.h>

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

//this function assumes strip start by going along the top row, down the left side, then back accross the bottom row and then up the right side to finish at the start
bool setStripColorAtPositionWithHeightCentered(state* currentState, int channelIndex, int led, uint32_t color){
  //LED max value should be half of width
  bool isWidthEven = currentState->dynamic.channels[channelIndex].width % 2 == 0;
  int topCenter = ceil(currentState->dynamic.channels[channelIndex].width / 2);
  int bottomCenter = currentState->dynamic.channels[channelIndex].height + currentState->dynamic.channels[channelIndex].width + topCenter - 1;
  int numberAddressablePositions = topCenter; //center pixel in the top row also tells us how many useable positions exist on the strip
  int offset = 0; //offset is used in case where width is even and middle is actually 2 leds instead of just 1

  if(isWidthEven){
    offset = 1;
  }

  if(led >= numberAddressablePositions){//led index is out of bounds
    return false;
  }

  if(led == 0){ //center led case
    if(isWidthEven){
      currentState->constant.strips[channelIndex]->setPixelColor(topCenter, color);// example 26 wide, halfwidth would be pixel 13
      currentState->constant.strips[channelIndex]->setPixelColor(topCenter+offset, color); //halfwidth + 1 would be pixel 14
      currentState->constant.strips[channelIndex]->setPixelColor(bottomCenter, color);
      currentState->constant.strips[channelIndex]->setPixelColor(bottomCenter+offset, color);
    }else{
      currentState->constant.strips[channelIndex]->setPixelColor(topCenter, color); //setting just one pixel color since it was an odd width
      currentState->constant.strips[channelIndex]->setPixelColor(bottomCenter, color); //setting just one pixel color since it was an odd width
    }
  }else{
    currentState->constant.strips[channelIndex]->setPixelColor(topCenter-led, color);// example 26 wide,  led is 4, halfwidth would be pixel 13 -4 should be pixel 9
    currentState->constant.strips[channelIndex]->setPixelColor(topCenter+offset+led, color); //halfwidth + 1 would be pixel 14 + led which is 4 would be 18
    currentState->constant.strips[channelIndex]->setPixelColor(bottomCenter-led, color);
    currentState->constant.strips[channelIndex]->setPixelColor(bottomCenter+offset+led, color);
    
    if(led == numberAddressablePositions - 1){//at the end of the width of the strip, need to turn on all side leds
      for(int i=(topCenter+offset+led+1); i<(bottomCenter-led); i++){ //start at last led in the left end of top row and add 1 to it so we start going down the side until we get to the left most led of the bottom row
        currentState->constant.strips[channelIndex]->setPixelColor(i, color); //this should handle all leds on left side of strip
      }
      
      for(int i=(bottomCenter+offset+led); i<(currentState->dynamic.channels[channelIndex].width); i++){//all pixels passed the last led on the right side of the bottom row is assumed to just be the side leds
        currentState->constant.strips[channelIndex]->setPixelColor(i, color); //this should handle all leds on right side of strip
      }
    }
  }
  return true;
}

bool setStripColorAtPosition(state* currentState, int row, int col, int pos, uint32_t color){
  bool updated = false;
  if( row <= 0 || col <= 0 || pos <= 0){
    return updated;
  }
  
  for(int channel=0; channel < MAX_CHANNELS; channel++){ //loop over all channels to see if they are in this row and column
    int channelIndex = currentState->dynamic.mappedPositions[row][col][channel] - 1; //we added 1 to the index so we didn't lose it when storing, removing it here.  stores 1 or 0 if the channel is in this position
    
    if(channelIndex >= 0){//channel is in specified position
      if(isChannelActive(currentState, channelIndex) == true){//this channel is active and accepts updates
        if(currentState->dynamic.channels[channelIndex].isInterior == false || (currentState->dynamic.channels[channelIndex].isInterior == true && getInteriorSwitchState() == true)){ //channel is not interior, or if it is the interior switch is on
           if(pos < getNumUseablePositions(currentState, channelIndex)){// channel has an addressable led at that position
            if(currentState->dynamic.channels[channelIndex].height > 1 && currentState->dynamic.channels[channelIndex].isCentered == true){//updates on this channel start in the center and work towards the end
              updated = setStripColorAtPositionWithHeightCentered(currentState, channelIndex, pos, color);
            }else if(currentState->dynamic.channels[channelIndex].height > 1 && currentState->dynamic.channels[channelIndex].isCentered == false){
              updated = setStripColorAtPositionWithHeightCentered(currentState, channelIndex, pos, color);// make function for this case
            }else if(currentState->dynamic.channels[channelIndex].height = 0 && currentState->dynamic.channels[channelIndex].isCentered == true){
              currentState->constant.strips[channelIndex]->setPixelColor(pos, color);//make function for this case
              updated = true;
            }else{
              currentState->constant.strips[channelIndex]->setPixelColor(pos, color);
              updated = true;
            }
          }
        }
      }
    }
  }
  return updated;
}


bool setStripColorAtPositionAcrossColumns(state* currentState, int row, int pos, uint32_t color){
  bool updated = false;
  for(int col=0; col < NUM_COLS; col++){
    bool hadUpdate = setStripColorAtPosition(currentState, row, col, pos, color);
    if(hadUpdate){
      updated = true;
    }
  }
  return updated;
}
