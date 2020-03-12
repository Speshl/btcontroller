#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include "defines.h"
#include "strips.h"
#include "bluetooth.h"
#include "states.h"

void getColors(state* currentState, bool alternate, uint32_t* primaryColor, uint32_t* secondaryColor){
  if(alternate){
    if(currentState->temp.stepIndex == 1){
      *primaryColor = currentState->constant.strips[0]->Color(currentState->dynamic.command.secondaryRed, currentState->dynamic.command.secondaryGreen, currentState->dynamic.command.secondaryBlue);
      *secondaryColor = currentState->constant.strips[0]->Color(currentState->dynamic.command.primaryRed, currentState->dynamic.command.primaryGreen, currentState->dynamic.command.primaryBlue);
      currentState->temp.stepIndex = 0;
    }else{
      currentState->temp.stepIndex = 1;
      *primaryColor = currentState->constant.strips[0]->Color(currentState->dynamic.command.primaryRed, currentState->dynamic.command.primaryGreen, currentState->dynamic.command.primaryBlue);
      *secondaryColor = currentState->constant.strips[0]->Color(currentState->dynamic.command.secondaryRed, currentState->dynamic.command.secondaryGreen, currentState->dynamic.command.secondaryBlue);
    }
  }else{
    *primaryColor = currentState->constant.strips[0]->Color(currentState->dynamic.command.primaryRed, currentState->dynamic.command.primaryGreen, currentState->dynamic.command.primaryBlue);
    *secondaryColor = currentState->constant.strips[0]->Color(currentState->dynamic.command.secondaryRed, currentState->dynamic.command.secondaryGreen, currentState->dynamic.command.secondaryBlue);
  }
}

void staticCommand(state* currentState, bool alternate){
  Serial.println("Starting static command");
  uint32_t primaryColor;
  uint32_t secondaryColor;
  getColors(currentState, alternate, &primaryColor, &secondaryColor);
  fillStrips(currentState, primaryColor);
  bool newCommand = delayAndPoll(currentState, currentState->dynamic.command.stepDelay);//delay should happen before next pixel changes
  if(newCommand){
    return;
  }
  Serial.println("Finished static command");
}

/*void pulseCommand(state* currentState){
  Serial.println("Starting pulse command");
  clearStrips();
  
  bool atTarget = false;
  int curRed = 0;
  int curGreen = 0;
  int curBlue = 0;

  while(!atTarget){//up Primary
    if(curRed < currentState.dynamic.command.primaryRed){
      curRed++;
    }
    if(curGreen < currentState.dynamic.command.primaryGreen){
      curGreen++;
    }
    if(curBlue < currentState.dynamic.command.primaryBlue){
      curBlue++;
    }
    if(curRed == currentState.dynamic.command.primaryRed && curGreen == currentState.dynamic.command.primaryGreen && curBlue == currentState.dynamic.command.primaryBlue){
      atTarget = true;
    }
    uint32_t color = currentState.constant.strips[0]->Color(curRed, curGreen, curBlue);
    for(int i=0; i<MAX_CHANNELS; i++){
      if(isChannelActive(i) == true){
        for(int j=0; j<currentState.dynamic.channels[i].numLEDs; j++){
          currentState.constant.strips[i]->setPixelColor(j, color);
        }
        currentState.constant.strips[i]->show();
      }
    }
    bool newCommand = delayAndPoll(currentState.dynamic.command.stepDelay);//delay should happen before next pixel changes
    if(newCommand){
      return;
    }
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
    uint32_t color = currentState.constant.strips[0]->Color(curRed, curGreen, curBlue);
    for(int i=0; i<MAX_CHANNELS; i++){
      if(isChannelActive(i) == true){
        for(int j=0; j<currentState.dynamic.channels[i].numLEDs; j++){
          currentState.constant.strips[i]->setPixelColor(j, color);
        }
        currentState.constant.strips[i]->show();
      }
    }
    delayAndPoll(currentState.dynamic.command.stepDelay);
  }
    atTarget = false;
    while(!atTarget){//up Secondary
    if(curRed < currentState.dynamic.command.secondaryRed){
      curRed++;
    }
    if(curGreen < currentState.dynamic.command.secondaryGreen){
      curGreen++;
    }
    if(curBlue < currentState.dynamic.command.secondaryBlue){
      curBlue++;
    }
    if(curRed == currentState.dynamic.command.secondaryRed && curGreen == currentState.dynamic.command.secondaryGreen && curBlue == currentState.dynamic.command.secondaryBlue){
      atTarget = true;
    }
    uint32_t color = currentState.constant.strips[0]->Color(curRed, curGreen, curBlue);
    for(int i=0; i<MAX_CHANNELS; i++){
      if(isChannelActive(i) == true){
        for(int j=0; j<currentState.dynamic.channels[i].numLEDs; j++){
          currentState.constant.strips[i]->setPixelColor(j, color);
        }
        currentState.constant.strips[i]->show();
      }
    }
    bool newCommand = delayAndPoll(currentState.dynamic.command.stepDelay);//delay should happen before next pixel changes
    if(newCommand){
      return;
    }
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
    uint32_t color = currentState.constant.strips[0]->Color(curRed, curGreen, curBlue);
    for(int i=0; i<MAX_CHANNELS; i++){
      if(isChannelActive(i) == true){
        for(int j=0; j<currentState.dynamic.channels[i].numLEDs; j++){
          currentState.constant.strips[i]->setPixelColor(j, color);
        }
        currentState.constant.strips[i]->show();
      }
    }
    bool newCommand = delayAndPoll(currentState.dynamic.command.stepDelay);//delay should happen before next pixel changes
    if(newCommand){
      return;
    }
  }
  Serial.println("Finished pulse command");
}*/

void waveCommand(state* currentState, bool alternate){
  Serial.println("Started wave command");
  uint32_t primaryColor;
  uint32_t secondaryColor;
  getColors(currentState, alternate, &primaryColor, &secondaryColor);
  fillStrips(currentState, primaryColor);

  for(int row=0; row<NUM_ROWS; row++){
    Serial.println("Switched rows, showing all strips");
    showAllStrips(currentState);
    int longestChannel = getLongestChannel(currentState,row);
    for(int led=0; led<longestChannel; led++){
      bool changed = false;
      for(int col=0; col<NUM_COLS; col++){
        for(int channel=0; channel < MAX_CHANNELS; channel++){
          int channelIndex = currentState->dynamic.mappedPositions[row][col][channel] - 1; //we added 1 to the index so we didn't lose it when storing, removing it here
          if(channelIndex >= 0 && isChannelActive(currentState,channelIndex) == true){//channelIndex will be -1 if there was no value stored there
              changed = true;
             if(currentState->dynamic.channels[channelIndex].numLEDs >= led){ //not at end of this strip
                currentState->constant.strips[channelIndex]->setPixelColor(led, secondaryColor);
                if(led != 0){
                  currentState->constant.strips[channelIndex]->setPixelColor(led-1, primaryColor); //set previous led back to primary color
                }
                Serial.print("Setting channel");
                Serial.print(channelIndex);
                Serial.print("'s ");
                Serial.print(led);
                Serial.println(" led to secondary color, and previous pixel to primary color");
             }else{
                currentState->constant.strips[channelIndex]->setPixelColor(currentState->dynamic.channels[channelIndex].numLEDs-1, primaryColor); //set last pixel in the strip back to primary Color
                Serial.print("Setting channel ");
                Serial.print(channelIndex);
                Serial.println("'s last pixel back to primaryColor");
             }
             currentState->constant.strips[channelIndex]->show();
             if(led == longestChannel-1){
              currentState->constant.strips[channelIndex]->setPixelColor(led, primaryColor);//go ahead and set the color back to primary on longest strip, will update all strips to set so we don't have to remember which strips
              Serial.print("Setting channel");
              Serial.print(channelIndex);
              Serial.println("'s last pixel back to primaryColor before switching rows");
             }
          }
        }
      }
      if(changed){
        bool newCommand = delayAndPoll(currentState, currentState->dynamic.command.stepDelay);//delay should happen before next pixel changes
        if(newCommand){
          return;
        }
      }
    }
  }
  Serial.println("Finished wave command");
}

void rollCommand(state* currentState, bool alternate){
  Serial.println("Started roll command");
  uint32_t primaryColor;
  uint32_t secondaryColor;
  getColors(currentState, alternate, &primaryColor, &secondaryColor);
  
  fillStrips(currentState, primaryColor);

  for(int row=0; row<NUM_ROWS; row++){
    Serial.println("Switched rows, showing all strips");
    showAllStrips(currentState);
    int longestChannel = getLongestChannel(currentState, row);
    for(int led=0; led<longestChannel; led++){
      bool changed = false;
      for(int col=0; col<NUM_COLS; col++){
        for(int channel=0; channel < MAX_CHANNELS; channel++){
          int channelIndex = currentState->dynamic.mappedPositions[row][col][channel] - 1; //we added 1 to the index so we didn't lose it when storing, removing it here
          if(channelIndex >= 0 && isChannelActive(currentState, channelIndex) == true){//channelIndex will be -1 if there was no value stored there
              changed = true;
             if(currentState->dynamic.channels[channelIndex].numLEDs >= led){ //not at end of this strip
                currentState->constant.strips[channelIndex]->setPixelColor(led, secondaryColor);
                Serial.print("Setting channel");
                Serial.print(channelIndex);
                Serial.print("'s ");
                Serial.print(led);
                Serial.println(" led to secondary color");
             }
             currentState->constant.strips[channelIndex]->show();
          }
        }
      }
      if(changed){
        bool newCommand = delayAndPoll(currentState, currentState->dynamic.command.stepDelay);//delay should happen before next pixel changes
        if(newCommand){
          return;
        }
      }
    }
  }
  Serial.println("Finished roll command");
}

void stackCommand(state* currentState, bool alternate){
  Serial.println("Started stack command");
  uint32_t primaryColor = 0;
  uint32_t secondaryColor = 0;

  if(alternate){//will alterante which color is stacked after a full cycl
    if( currentState->temp.lastRowUsed == 0 && currentState->temp.lastLEDUsed == 0){//completed a stack
      Serial.println("Swapping Primary Color");
      if(currentState->temp.stepIndex == 1){
        Serial.println("Switching to alternate colors");
        currentState->temp.stepIndex = 0;
      }else{
        Serial.println("Switching to standard colors");
        currentState->temp.stepIndex = 1;
      } 
    }
  }

  if(currentState->temp.stepIndex == 0 || alternate == false){
    Serial.println("Using alternate colors");
    primaryColor = currentState->constant.strips[0]->Color(currentState->dynamic.command.secondaryRed, currentState->dynamic.command.secondaryGreen, currentState->dynamic.command.secondaryBlue);
    secondaryColor = currentState->constant.strips[0]->Color(currentState->dynamic.command.primaryRed, currentState->dynamic.command.primaryGreen, currentState->dynamic.command.primaryBlue);
  }else{
    Serial.println("Using standard colors");
    primaryColor = currentState->constant.strips[0]->Color(currentState->dynamic.command.primaryRed, currentState->dynamic.command.primaryGreen, currentState->dynamic.command.primaryBlue);
    secondaryColor = currentState->constant.strips[0]->Color(currentState->dynamic.command.secondaryRed, currentState->dynamic.command.secondaryGreen, currentState->dynamic.command.secondaryBlue);
  }
  
  if( currentState->temp.lastRowUsed == 0 && currentState->temp.lastLEDUsed == 0){ //reset to last led in last row
    fillStrips(currentState, primaryColor);
    Serial.println("Getting last row and channel");
    currentState->temp.lastRowUsed = getLastRow(currentState);
    currentState->temp.lastLEDUsed = getLongestChannel(currentState, currentState->temp.lastRowUsed);
  }else{
    if(currentState->temp.lastLEDUsed == 0){
      Serial.print("Row ");
      Serial.print(currentState->temp.lastRowUsed);
      Serial.println(" complete, getting next row");
      
      for(int i=currentState->temp.lastRowUsed-1; i>= 0; i--){
        int nextChannel = getLongestChannel(currentState, i);
        if(nextChannel > 0){
          currentState->temp.lastRowUsed = i;
          currentState->temp.lastLEDUsed = nextChannel;
          Serial.print("Next row is ");
          Serial.println(currentState->temp.lastRowUsed);
          break;
        }
      }
    }else{
      Serial.println("Decrementing LED used");
      currentState->temp.lastLEDUsed--;
    }
  }
  
  Serial.print("Stacking Row ");
  Serial.println(currentState->temp.lastRowUsed);
  Serial.print("Stacking LED ");
  Serial.println(currentState->temp.lastLEDUsed-1);
  for(int row=0; row<NUM_ROWS; row++){
    if(row > currentState->temp.lastRowUsed){
      break;//don't need to update anymore
    }
    Serial.print("Updating row ");
    Serial.println(row);
    showAllStrips(currentState);
    int longestChannel = getLongestChannel(currentState, row);
    for(int led=0; led<longestChannel; led++){
      bool changed = false;//only want to delay if there was a change
      for(int col=0; col<NUM_COLS; col++){
        for(int channel=0; channel < MAX_CHANNELS; channel++){
          int channelIndex = currentState->dynamic.mappedPositions[row][col][channel] - 1; //we added 1 to the index so we didn't lose it when storing, removing it here.  stores 1 or 0 if the channel is in this position
          if(channelIndex >= 0 && isChannelActive(currentState, channelIndex) == true){//channelIndex will be -1 if there was no value stored there
             if(currentState->dynamic.channels[channelIndex].numLEDs >= led){ //not at end of this strip
                if(row <= currentState->temp.lastRowUsed){ //this row hasn't completed a stack yet
                  if(row == currentState->temp.lastRowUsed && led >= currentState->temp.lastLEDUsed){//this led is already stacked so do nothing
                    Serial.print("Channel ");
                    Serial.print(channelIndex);
                    Serial.print("'s ");
                    Serial.print(led);
                    Serial.println(" led is stacked");
                  }else{//this led hasn't been stacked yet so apply updates
                    changed = true;
                    currentState->constant.strips[channelIndex]->setPixelColor(led, secondaryColor); //set current pixel to secondary color
                    if(led != 0){
                      currentState->constant.strips[channelIndex]->setPixelColor(led-1, primaryColor); //set previous led back to primary color
                    }
                    Serial.print("Setting channel ");
                    Serial.print(channelIndex);
                    Serial.print("'s ");
                    Serial.print(led);
                    Serial.println(" led to secondary color, and previous pixel to primary color");
                  }
                }
             }else{//end of the strip if its not the longest strip in the row
               if(row < currentState->temp.lastRowUsed){//if this row is below the last row used              
                  currentState->constant.strips[channelIndex]->setPixelColor(currentState->dynamic.channels[channelIndex].numLEDs-1, primaryColor); //set last pixel in the strip back to primary Color, showing all strips at the start of next loops well display it and complete the animation
                  Serial.print("Setting channel ");
                  Serial.print(channelIndex);
                  Serial.println("'s last pixel back to primaryColor, not the longest channel in the row");
               }
             }
             currentState->constant.strips[channelIndex]->show();
             if(led == longestChannel-1){  //on last led in the longest strip and will be switching rows next
              if(row != currentState->temp.lastRowUsed){//stacking this led      
                changed = true;
                currentState->constant.strips[channelIndex]->setPixelColor(led, primaryColor);//go ahead and set the color back to primary on longest strip, will update all strips to set so we don't have to remember which strips
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
        bool newCommand = delayAndPoll(currentState, currentState->dynamic.command.stepDelay);//delay should happen before next pixel changes
        if(newCommand){
          return;
        }
      }
    }
  }
  Serial.println("Finished stack command");
}

void runCommand(state* currentState) {
  switch(currentState->dynamic.command.animation){
    case 0:
      staticCommand(currentState, false);
      break;
    case 1:
      staticCommand(currentState, true);
      break;
    /*case 2:
      pulseCommand(currentState);
      break;*/
    case 3:
      waveCommand(currentState, false);
      break;
    case 4:
      waveCommand(currentState, true);
      break;
    case 5:
      rollCommand(currentState, false);
      break;
    case 6:
      rollCommand(currentState, true);
      break;
    case 7:
      stackCommand(currentState, false);
      break;
    case 8:
      stackCommand(currentState, true);
      break;
    default:
      staticCommand(currentState, false);
  }
}
