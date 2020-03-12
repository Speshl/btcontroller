#include <Adafruit_NeoPixel.h>
#include "defines.h"
#include "states.h"
#include "display.h"
#include "bluetooth.h"
#include "strips.h"
#include "commands.h"

void setup() {
  pinMode(interiorTogglePin, INPUT);
  Serial.begin(57600);
  //while(!Serial);//temporary stops running until serial connection is made
  state *currentState = getCurrentState();
  setInitialTempState(&(currentState->temp));
  setInitialDynamicState(&(currentState->dynamic));
  describeDynamicState(&(currentState->dynamic));
  setupService(&(currentState->constant));
  setInitialCharacteristicValues(currentState);
  setInitialStrips(currentState);
  Serial.println("Ready!");
}

void loop() {
  state *currentState = getCurrentState();
  // put your main code here, to run repeatedly:
  delayAndPoll(currentState, 0);
  runCommand(currentState);
}
