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

void breathCommand(state* currentState, bool alternate){
  Serial.println("Starting breath command");
  commandState command = getCommand(currentState);
  uint32_t primaryColor;
  uint32_t secondaryColor;
  getColors(currentState, alternate, &primaryColor, &secondaryColor);
  
  uint32_t currentColor;
  byte currentRed = 0;
  byte currentGreen = 0;
  byte currentBlue = 0;

  secondaryColor = currentState->constant.strips[0]->Color(currentRed, currentGreen, currentBlue);

  for(int i=0; i<256; i++){ //at most 256 steps between no color and target color (GOING UP)
    if(currentRed < command.primaryRed){
      currentRed++;
    }
    if(currentGreen < command.primaryGreen){
      currentGreen++;
    }
    if(currentBlue < command.primaryBlue){
      currentBlue++;
    }
    currentColor = currentState->constant.strips[0]->Color(currentRed, currentGreen, currentBlue);
    fillStrips(currentState, currentColor);
    
    bool newCommand = delayAndPoll(currentState, command.stepDelay);//delay should happen before next pixel changes
    if(newCommand){
      return;
    }
    
    if(currentColor == primaryColor){//At target color
      break;//at target so stop going up
    }
  }

  for(int i=0; i<256; i++){ //at most 256 steps between no color and target color (GOING DOWN)
    if(currentRed > 0){
      currentRed--;
    }
    if(currentGreen > 0){
      currentGreen--;
    }
    if(currentBlue > 0){
      currentBlue--;
    }
    currentColor = currentState->constant.strips[0]->Color(currentRed, currentGreen, currentBlue);
    fillStrips(currentState, currentColor);
    
    bool newCommand = delayAndPoll(currentState, command.stepDelay);//delay should happen before next pixel changes
    if(newCommand){
      return;
    }
    
    if(currentColor == secondaryColor){//At target color (off)
      break;//at target so stop going up
    }
  }
  
}

void waveCommand(state* currentState, bool alternate){
  Serial.println("Started wave command");
  commandState command = getCommand(currentState);
  uint32_t primaryColor;
  uint32_t secondaryColor;
  getColors(currentState, alternate, &primaryColor, &secondaryColor);
  fillStrips(currentState, primaryColor);
  
  for(int row=0; row<NUM_ROWS; row++){
    int mostUseablePositionsInRow = getLongestChannelInRow(currentState, row);
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

void rollCommand(state* currentState, bool alternate){
  Serial.println("Started roll command");
  commandState command = getCommand(currentState);
  uint32_t primaryColor;
  uint32_t secondaryColor;
  getColors(currentState, alternate, &primaryColor, &secondaryColor);
  fillStrips(currentState, primaryColor);

  for(int row=0; row<NUM_ROWS; row++){
    int mostUseablePositionsInRow = getLongestChannelInRow(currentState, row);
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

void stackCommand(state* currentState, bool alternate){
  Serial.println("Started stack command");
  commandState command = getCommand(currentState);
  uint32_t primaryColor = 0;
  uint32_t secondaryColor = 0;

  if(alternate){//will alterante which color is stacked after a full cycl
    if( currentState->temp.lastRowUsed == 0 && currentState->temp.lastPosUsed == 0){//completed a stack
      //Serial.println("Swapping Primary Color");
      if(currentState->temp.stepIndex == 1){
        //Serial.println("Using standard colors");
        currentState->temp.stepIndex = 0;
      }else{
       // Serial.println("Using alternate colors");
        currentState->temp.stepIndex = 1;
      } 
    }
  }

  if(currentState->temp.stepIndex == 1 && alternate == true){
    //Serial.println("Using alternate colors");
    primaryColor = currentState->constant.strips[0]->Color(command.secondaryRed, command.secondaryGreen, command.secondaryBlue);
    secondaryColor = currentState->constant.strips[0]->Color(command.primaryRed, command.primaryGreen, command.primaryBlue);
  }else{
    //Serial.println("Using standard colors");
    primaryColor = currentState->constant.strips[0]->Color(command.primaryRed, command.primaryGreen, command.primaryBlue);
    secondaryColor = currentState->constant.strips[0]->Color(command.secondaryRed, command.secondaryGreen, command.secondaryBlue);
  }
  
  if( currentState->temp.lastRowUsed == 0 && currentState->temp.lastPosUsed == 0){ //reset to last led in last row
    fillStrips(currentState, primaryColor);
    //Serial.println("Getting last row and channel");
    currentState->temp.lastRowUsed = getLastRow(currentState);
    currentState->temp.lastPosUsed = getLongestChannelInRow(currentState, currentState->temp.lastRowUsed);
  }else{
    if(currentState->temp.lastPosUsed == 0){
      //Serial.print("Row ");
      //Serial.print(currentState->temp.lastRowUsed);
      //Serial.println(" complete, getting next row");
      
      for(int i=currentState->temp.lastRowUsed-1; i>= 0; i--){
        int nextChannel = getLongestChannelInRow(currentState, i);
        if(nextChannel > 0){
          currentState->temp.lastRowUsed = i;
          currentState->temp.lastPosUsed = nextChannel;
          //Serial.print("Next row is ");
          //Serial.println(currentState->temp.lastRowUsed);
          break;
        }
      }
    }else{
      //Serial.println("Decrementing LED used");
      currentState->temp.lastPosUsed--;
    }
  }
  
  //Serial.print("Stacking Row ");
  //Serial.println(currentState->temp.lastRowUsed);
  //Serial.print("Stacking LED ");
  //Serial.println(currentState->temp.lastPosUsed);
  
  for(int row=0; row<NUM_ROWS; row++){
    if(row > currentState->temp.lastRowUsed){
      break;//don't need to update anymore
    }
    int mostUseablePositionsInRow = getLongestChannelInRow(currentState, row);
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

void signalCommand(state* currentState){
  //need to store light state of strips we are changing so we can put them back after the signal is disabled
  uint32_t brakeColor = currentState->constant.strips[0]->Color(255, 0, 0);
  uint32_t turnColor = currentState->constant.strips[0]->Color(255, 125, 0);
  uint32_t offColor = currentState->constant.strips[0]->Color(0, 0, 0);
  bool gotUpdate = false;

  if(currentState->temp.brakeLight == true && currentState->temp.leftTurnLight == true){// brake and left turn

    for(int i=1; i<NUM_COLS; i++){ //start at 1 so we don't turn on brake instead of turn signal on left side
      setStripColorAtLocation(currentState, NUM_ROWS-1,i,brakeColor);
    }
    
    for(int i=0; i<NUM_ROWS; i++){ //turn signal on
      setStripColorAtLocation(currentState, i,0,turnColor);
    }

    gotUpdate = delayAndPoll(currentState, SIGNAL_DELAY);// delay with signal on
    if(gotUpdate == true){
      return;
    }

    for(int i=0; i<NUM_ROWS; i++){//turn signal off
      setStripColorAtLocation(currentState, i,0,offColor);
    }

    setStripColorAtLocation(currentState, NUM_ROWS-1, 0, brakeColor); //turn brake light back on where turn signal went off
    gotUpdate = delayAndPoll(currentState, SIGNAL_DELAY);// delay with signal on
    if(gotUpdate == true){
      return;
    }
    
  }else if(currentState->temp.brakeLight == true && currentState->temp.rightTurnLight == true){// brake and right turn
    for(int i=0; i<NUM_COLS-1; i++){ //stop at cols-1 so we don't turn on brake instead of turn signal on right side
      setStripColorAtLocation(currentState, NUM_ROWS-1, i, brakeColor);
    }
    
    for(int i=0; i<NUM_ROWS; i++){ //turn signal on
      setStripColorAtLocation(currentState, i, NUM_COLS-1, turnColor);
    }
    
    gotUpdate = delayAndPoll(currentState, SIGNAL_DELAY);// delay with signal on
    if(gotUpdate == true){
      return;
    }

    for(int i=0; i<NUM_ROWS; i++){//turn signal off
      setStripColorAtLocation(currentState, i, NUM_COLS-1, offColor);
    }

    setStripColorAtLocation(currentState, NUM_ROWS-1, NUM_COLS-1, brakeColor); //turn brake light back on where turn signal went off
    gotUpdate = delayAndPoll(currentState, SIGNAL_DELAY);// delay with signal on
    if(gotUpdate == true){
      return;
    }
    
  }else if(currentState->temp.leftTurnLight == true){//left turn
    for(int i=0; i<NUM_ROWS; i++){
      setStripColorAtLocation(currentState, i, 0, turnColor);
    }
    
    gotUpdate = delayAndPoll(currentState, SIGNAL_DELAY);// delay with signal on
    if(gotUpdate == true){
      return;
    }

    for(int i=0; i<NUM_ROWS; i++){
      setStripColorAtLocation(currentState, i, 0, offColor);
    }
    
    gotUpdate = delayAndPoll(currentState, SIGNAL_DELAY);// delay with signal on
    if(gotUpdate == true){
      return;
    }
  }else if(currentState->temp.rightTurnLight == true){//right turn
    for(int i=0; i<NUM_ROWS; i++){
      setStripColorAtLocation(currentState, i, NUM_COLS-1, turnColor);
    }
    
    gotUpdate = delayAndPoll(currentState, SIGNAL_DELAY);// delay with signal on
    if(gotUpdate == true){
      return;
    }

    for(int i=0; i<NUM_ROWS; i++){
      setStripColorAtLocation(currentState, i, NUM_COLS-1, offColor);
    }
    
    gotUpdate = delayAndPoll(currentState, SIGNAL_DELAY);// delay with signal on
    if(gotUpdate == true){
      return;
    }
  }else if(currentState->temp.brakeLight == true){//brake
    for(int i=0; i<NUM_COLS; i++){
      setStripColorAtLocation(currentState, NUM_ROWS-1, i, brakeColor);
    }
  }
}

void runCommand(state* currentState) {
  commandState command = getCommand(currentState);
  /*if(currentState->temp.brakeLight == true || currentState->temp.leftTurnLight == true || currentState->temp.rightTurnLight == true){//handle signals
    signalCommand(currentState);
  }else{*/
  if(getInteriorLightState(currentState) == true){
    Serial.println("Interior Lights On");
  }else{
    Serial.println("Interior Lights Off");
  }
    switch(command.animation){
      case 0:
        staticCommand(currentState, false);
        break;
      case 1:
        staticCommand(currentState, true);
        break;
      case 2:
        breathCommand(currentState, false);
        break;
      case 3:
        breathCommand(currentState, true);
        break;
      case 4:
        waveCommand(currentState, false);
        break;
      case 5:
        waveCommand(currentState, true);
        break;
      case 6:
        rollCommand(currentState, false);
        break;
      case 7:
        rollCommand(currentState, true);
        break;
      case 8:
        stackCommand(currentState, false);
        break;
      case 9:
        stackCommand(currentState, true);
        break;
      default:
        staticCommand(currentState, false);
    }
  //}
}
