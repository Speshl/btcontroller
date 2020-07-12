#include <Arduino.h>
#include <FlashStorage.h>
#include <ArduinoBLE.h>
#include "defines.h"
#include "states.h"
#include "display.h"
#include "strips.h"

void updateBulkReadChar(state* currentState){
  bulkReadUnion bulkReadConverter;
  bulkReadConverter.packet.on = currentState->temp.lightsOn;
  bulkReadConverter.packet.interiorOn = currentState->temp.interiorOn;
  bulkReadConverter.packet.command = currentState->dynamic.command;
  memcpy(bulkReadConverter.packet.channels, currentState->dynamic.channels, sizeof(currentState->dynamic.channels));
  //bulkReadConverter.packet.channels = currentState->dynamic.channels;
  currentState->constant.bulkRead->writeValue(bulkReadConverter.serializedArray, sizeof(bulkReadConverter.serializedArray));
}

void setInitialCharacteristicValues(state* currentState){
  Serial.println("Start setting initial service values");

  currentState->constant.on->writeValue(currentState->temp.lightsOn);
  currentState->constant.interiorOn->writeValue(currentState->temp.interiorOn);
  
  commandUnion commandConverter;
  commandConverter.packet = currentState->dynamic.command;
  currentState->constant.command->writeValue(commandConverter.serializedArray, sizeof(commandConverter.serializedArray));

  channelUnion channelConverter;
  for(int i=0; i<MAX_CHANNELS; i++){
    channelConverter.packet = currentState->dynamic.channels[i];
    currentState->constant.channels[i]->writeValue(channelConverter.serializedArray, sizeof(channelConverter.serializedArray));
  }
  updateBulkReadChar(currentState);
  
  Serial.println("Finish setting initial service values");
}

//todo: Update this to check for central and wait while connected. Currently times out when connecting to bluetooth
bool delayAndPoll(state* currentState, int sleep){
  //Serial.println("Polling for state change");
  bool hasUpdated = false;
  unsigned long ulSleep = (unsigned long) sleep;
  unsigned long currentTime = millis();
  unsigned long targetTime = currentTime + ulSleep;
  while(targetTime > millis()){
    BLE.poll();
    /*if(BLE.connected()){
      Serial.println("Central is connected");
      BLEDevice central = BLE.central();
      if(central){
        Serial.print("Connected to central: ");
        Serial.println(central.address());
      }
    }*/
    bool previousSignalState = currentState->temp.lightsSaved; //this is true when a signal is on
    bool currentSignalState = updateSignals(currentState); //updates signals and returns bool for if a signal is on
    if(previousSignalState != currentSignalState){
      return true;
    }
    
    hasUpdated = updateTempState(currentState);
    if(currentState->dynamic.hasUpdated == true || hasUpdated == true){
      Serial.println("Found new updated command, resetting temp command state.");
      clearStrips(&(currentState->constant));
      currentState->temp.stepIndex = 0;
      currentState->dynamic.hasUpdated = false;
      hasUpdated = true;
      return hasUpdated;//immediately drop out to process new command
    }
  }
  return hasUpdated;
}

void saveDynamicState(BLEDevice central, BLECharacteristic characteristic){
  state* currentState = getCurrentState();
  if( currentState->constant.save->value() == true){
    Serial.println("Started saving dynamic state");
    saveState(currentState->dynamic);
    currentState->constant.save->writeValue(false); //set back to false when finished to signify completion
    Serial.println("Finished saving dynamic state");
  }
} 

void toggleLights(BLEDevice central, BLECharacteristic characteristic){
  state* currentState = getCurrentState();
  Serial.println("New light state value found");
  if(currentState->constant.on->value() == true){
    Serial.println("Turning lights on");
    currentState->temp.lightsOn = true;
  }else{
    Serial.println("Turning lights off");
    currentState->temp.lightsOn = false;
  }
  updateBulkReadChar(currentState);
}

void toggleInteriorLights(BLEDevice central, BLECharacteristic characteristic){
  state* currentState = getCurrentState();
  Serial.println("New interior light stats value found");
  if(currentState->constant.interiorOn->value() == true){
    Serial.println("Turning interior lights on");
    currentState->temp.interiorOn = true;
  }else{
    Serial.println("Turning interior lights off");
    currentState->temp.interiorOn = false;
  }
  updateBulkReadChar(currentState);
}

void updateCommand(BLEDevice central, BLECharacteristic characteristic){
  state* currentState = getCurrentState();
  Serial.println("Recieved new command state");
  commandUnion converter;
  currentState->constant.command->readValue(converter.serializedArray,sizeof(commandState));
  currentState->dynamic.command = converter.packet;
  currentState->dynamic.hasUpdated = true;
  Serial.println("Applied new command state");
  describeCommandState(&(currentState->dynamic));
  updateBulkReadChar(currentState);
}

void updateChannel(BLEDevice central, BLECharacteristic characteristic){
  state* currentState = getCurrentState();
  Serial.println("Recieved new channel state");
  int channelIndex = 0;
  for(int i=0; i<MAX_CHANNELS; i++){//determine which channel was updated
    if(characteristic.uuid() == characteristicUUIDs[i+3]){
      channelIndex = i;
      Serial.println("Matching channel UUID found for update");
    }
  }
  
  channelUnion converter;
  currentState->constant.channels[channelIndex]->readValue(converter.serializedArray, sizeof(channelState));
  currentState->dynamic.channels[channelIndex] = converter.packet; 
  setStrip(currentState, channelIndex);
  currentState->dynamic.hasUpdated = true;
  mapPositions(&(currentState->dynamic));
  Serial.println("Applied new channel state");
  describeChannelState(&(currentState->dynamic),channelIndex);
  updateBulkReadChar(currentState);
}

void setupService(staticState* sState){
  Serial.println("Starting BLE");
  while (!BLE.begin()) {
    Serial.println("Waiting for BLE to start");
    delay(1);
  }
  Serial.println("Start building service");
  BLE.setDeviceName("SquadGlowController");
  BLE.setLocalName("SquadGlowController");
  sState->data = new BLEService(serviceUUID);
  
  sState->save = new BLEBoolCharacteristic(characteristicUUIDs[0], BLERead | BLEWrite);
  sState->save->setEventHandler(BLEWritten, saveDynamicState);
  sState->data->addCharacteristic(*sState->save);
  
  sState->on = new BLEBoolCharacteristic(characteristicUUIDs[1], BLERead | BLEWrite);
  sState->on->setEventHandler(BLEWritten, toggleLights);
  sState->data->addCharacteristic(*sState->on);
  
  sState->interiorOn = new BLEBoolCharacteristic(characteristicUUIDs[12], BLERead | BLEWrite);
  sState->interiorOn->setEventHandler(BLEWritten, toggleInteriorLights);
  sState->data->addCharacteristic(*sState->interiorOn);

  sState->command = new BLECharacteristic(characteristicUUIDs[2], BLERead | BLEWrite, sizeof(commandState), 512);
  sState->command->setEventHandler(BLEWritten, updateCommand);
  sState->data->addCharacteristic(*sState->command);
  
  sState->bulkRead = new BLECharacteristic(characteristicUUIDs[3], BLERead, sizeof(bulkRead), 512);
  sState->data->addCharacteristic(*sState->bulkRead);

  for(int i=0; i<MAX_CHANNELS; i++){
    sState->channels[i] = new BLECharacteristic(characteristicUUIDs[4+i], BLERead | BLEWrite, sizeof(channelState), 100); //index +4 since channels start at 4th
    sState->channels[i]->setEventHandler(BLEWritten, updateChannel);
    sState->data->addCharacteristic(*sState->channels[i]);
  }
  BLE.addService(*sState->data);
  BLE.setAdvertisedService(*sState->data);
  BLE.advertise();//I think only 1 is advertised at a time
  Serial.println("Finish building service");
}
