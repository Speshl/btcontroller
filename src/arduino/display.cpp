#include <Arduino.h>
#include "defines.h"

void describeCommandState(dynamicState *dState){
  Serial.println("Start Describing Command State - ");
  
  Serial.print("Primary Red: ");
  Serial.println(dState->command.primaryRed);

  Serial.print("Primary Green: ");
  Serial.println(dState->command.primaryGreen);

  Serial.print("Primary Blue: ");
  Serial.println(dState->command.primaryBlue);

  Serial.print("Secondary Red: ");
  Serial.println(dState->command.secondaryRed);

  Serial.print("Secondary Green: ");
  Serial.println(dState->command.secondaryGreen);

  Serial.print("Secondary Blue: ");
  Serial.println(dState->command.secondaryBlue);

  Serial.print("Animation: ");
  Serial.println(dState->command.animation);

  Serial.print("Step Delay: ");
  Serial.println(dState->command.stepDelay);

  Serial.println("Finish Describing Command State");
}

void describeChannelState(dynamicState *dState, int channelIndex){
  Serial.print("Start Describing Channel ");
  Serial.print(channelIndex);
  Serial.println(" - ");

  Serial.print("Strip Is Interior: ");
  Serial.println(dState->channels[channelIndex].isInterior);

  Serial.print("Strip Used: ");
  Serial.println(dState->channels[channelIndex].stripUsed);

  Serial.print("Strip Type: ");
  Serial.println(dState->channels[channelIndex].stripType);

  Serial.print("Strip Order: ");
  Serial.println(dState->channels[channelIndex].stripOrder);

  Serial.print("Strip Position: ");
  Serial.println(dState->channels[channelIndex].stripPosition);

  Serial.print("Number LEDs: ");
  Serial.println(dState->channels[channelIndex].numLEDs);

  Serial.print("Height: ");
  Serial.println(dState->channels[channelIndex].height);

  Serial.print("Width: ");
  Serial.println(dState->channels[channelIndex].width);
}

void describeDynamicState(dynamicState *dState){
  describeCommandState(dState);
  for(int i=0; i<MAX_CHANNELS; i++){
    describeChannelState(dState, i);
  }
}
