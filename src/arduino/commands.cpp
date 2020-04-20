#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include "defines.h"
#include "strips.h"
#include "bluetooth.h"
#include "states.h"

void getColors(state* currentState, bool alternate, uint32_t* primaryColor, uint32_t* secondaryColor){
  commandState command = getCommand(currentState);
  if(alternate){
    if(currentState->temp.stepIndex == 1){
      *primaryColor = currentState->constant.strips[0]->Color(command.secondaryRed, command.secondaryGreen, command.secondaryBlue);
      *secondaryColor = currentState->constant.strips[0]->Color(command.primaryRed, command.primaryGreen, command.primaryBlue);
      currentState->temp.stepIndex = 0;
    }else{
      currentState->temp.stepIndex = 1;
      *primaryColor = currentState->constant.strips[0]->Color(command.primaryRed, command.primaryGreen, command.primaryBlue);
      *secondaryColor = currentState->constant.strips[0]->Color(command.secondaryRed, command.secondaryGreen, command.secondaryBlue);
    }
  }else{
    *primaryColor = currentState->constant.strips[0]->Color(command.primaryRed, command.primaryGreen, command.primaryBlue);
    *secondaryColor = currentState->constant.strips[0]->Color(command.secondaryRed, command.secondaryGreen, command.secondaryBlue);
  }
}

void staticCommand(state* currentState, bool alternate){
  Serial.println("Starting static command");
  commandState command = getCommand(currentState);
  uint32_t primaryColor;
  uint32_t secondaryColor;
  getColors(currentState, alternate, &primaryColor, &secondaryColor);
  fillStrips(currentState, primaryColor);
  bool newCommand = delayAndPoll(currentState, command.stepDelay);//delay should happen before next pixel changes
  if(newCommand){
    return;
  }
  Serial.println("Finished static command");
}

void waveCommand(state* currentState, bool alternate){
  Serial.println("Started wave command");
  commandState command = getCommand(currentState);
  uint32_t primaryColor;
  uint32_t secondaryColor;
  getColors(currentState, alternate, &primaryColor, &secondaryColor);
  fillStrips(currentState, primaryColor);
  
  for(int row=0; row<NUM_ROWS; row++){
    int mostUseablePositionsInRow = getLongestChannel(currentState, row);
    for(int pos=0; pos<mostUseablePositionsInRow; pos++){ //loop over all possible addressable positions in row
      bool changed = false;
      changed = setStripColorAtPositionAcrossColumns(currentState, row, pos, secondaryColor); //set current position to the wave's color
      if(changed){
        bool newCommand = delayAndPoll(currentState, command.stepDelay);//delay should happen before next pixel changes
        if(newCommand){
          return; //break out if new command detected
        }else{ //turn off leds we turned on after the delay
          setStripColorAtPositionAcrossColumns(currentState, row, pos, primaryColor); //set current position to the background color
        }
      }
    }
  }
  Serial.println("Finished wave command");
}

/*void old_waveCommand(state* currentState, bool alternate){
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
}*/

void rollCommand(state* currentState, bool alternate){
  Serial.println("Started roll command");
  commandState command = getCommand(currentState);
  uint32_t primaryColor;
  uint32_t secondaryColor;
  getColors(currentState, alternate, &primaryColor, &secondaryColor);
  fillStrips(currentState, primaryColor);

  for(int row=0; row<NUM_ROWS; row++){
    int mostUseablePositionsInRow = getLongestChannel(currentState, row);
    for(int pos=0; pos<mostUseablePositionsInRow; pos++){ //loop over all possible addressable positions in row
      bool changed = false;
      changed = setStripColorAtPositionAcrossColumns(currentState, row, pos, secondaryColor); //set current position to the wave's color
      if(changed){
        bool newCommand = delayAndPoll(currentState, command.stepDelay);//delay should happen before next pixel changes
        if(newCommand){
          return; //break out if new command detected
        }
      }
    }
  }
  Serial.println("Finished roll command");
}

/*void rollCommand(state* currentState, bool alternate){
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
}*/

void stackCommand(state* currentState, bool alternate){
  Serial.println("Started stack command");
  commandState command = getCommand(currentState);
  uint32_t primaryColor = 0;
  uint32_t secondaryColor = 0;

  if(alternate){//will alterante which color is stacked after a full cycl
    if( currentState->temp.lastRowUsed == 0 && currentState->temp.lastPosUsed == 0){//completed a stack
      Serial.println("Swapping Primary Color");
      if(currentState->temp.stepIndex == 1){
        Serial.println("Using standard colors");
        currentState->temp.stepIndex = 0;
      }else{
        Serial.println("Using alternate colors");
        currentState->temp.stepIndex = 1;
      } 
    }
  }

  if(currentState->temp.stepIndex == 1 && alternate == true){
    Serial.println("Using alternate colors");
    primaryColor = currentState->constant.strips[0]->Color(command.secondaryRed, command.secondaryGreen, command.secondaryBlue);
    secondaryColor = currentState->constant.strips[0]->Color(command.primaryRed, command.primaryGreen, command.primaryBlue);
  }else{
    Serial.println("Using standard colors");
    primaryColor = currentState->constant.strips[0]->Color(command.primaryRed, command.primaryGreen, command.primaryBlue);
    secondaryColor = currentState->constant.strips[0]->Color(command.secondaryRed, command.secondaryGreen, command.secondaryBlue);
  }
  
  if( currentState->temp.lastRowUsed == 0 && currentState->temp.lastPosUsed == 0){ //reset to last led in last row
    fillStrips(currentState, primaryColor);
    Serial.println("Getting last row and channel");
    currentState->temp.lastRowUsed = getLastRow(currentState);
    currentState->temp.lastPosUsed = getLongestChannel(currentState, currentState->temp.lastRowUsed);
  }else{
    if(currentState->temp.lastPosUsed == 0){
      Serial.print("Row ");
      Serial.print(currentState->temp.lastRowUsed);
      Serial.println(" complete, getting next row");
      
      for(int i=currentState->temp.lastRowUsed-1; i>= 0; i--){
        int nextChannel = getLongestChannel(currentState, i);
        if(nextChannel > 0){
          currentState->temp.lastRowUsed = i;
          currentState->temp.lastPosUsed = nextChannel;
          Serial.print("Next row is ");
          Serial.println(currentState->temp.lastRowUsed);
          break;
        }
      }
    }else{
      Serial.println("Decrementing LED used");
      currentState->temp.lastPosUsed--;
    }
  }
  
  Serial.print("Stacking Row ");
  Serial.println(currentState->temp.lastRowUsed);
  Serial.print("Stacking LED ");
  Serial.println(currentState->temp.lastPosUsed);
  
  for(int row=0; row<NUM_ROWS; row++){
    if(row > currentState->temp.lastRowUsed){
      break;//don't need to update anymore
    }
    int mostUseablePositionsInRow = getLongestChannel(currentState, row);
    for(int pos=0; pos<mostUseablePositionsInRow; pos++){ //loop over all possible addressable positions in row
      bool changed = false;//only want to delay if there was a change
      if(row == currentState->temp.lastRowUsed && pos >currentState->temp.lastPosUsed){
        break;//This position is already stacked so skip, should hit this until row ends
      }
      
      changed = setStripColorAtPositionAcrossColumns(currentState, row, pos, secondaryColor);
      if(changed){
        bool newCommand = delayAndPoll(currentState, command.stepDelay);//delay should happen before next pixel changes
        if(newCommand){
          return; //break out if new command detected
        }
      }

      if(row == currentState->temp.lastRowUsed && pos == currentState->temp.lastPosUsed){ //this is the position we are stacking
        break;//don't need to set the color back
      }else{
        setStripColorAtPositionAcrossColumns(currentState, row, pos, primaryColor);//set position back to primary color after the delay
      }
      
    }
  }
  Serial.println("Finished stack command");
}

void runCommand(state* currentState) {
  commandState command = getCommand(currentState);
  switch(command.animation){
    case 0:
      staticCommand(currentState, false);
      break;
    case 1:
      staticCommand(currentState, true);
      break;
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
