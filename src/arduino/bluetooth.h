void setInitialCharacteristicValues(state* currentState);

void updateBulkReadChar(state* currentState);

bool delayAndPoll(state* currentState, int sleep);

void setupService(staticState* sState);

void saveDynamicState(BLEDevice central, BLECharacteristic characteristic);

void toggleLights(BLEDevice central, BLECharacteristic characteristic);

void updateCommand(BLEDevice central, BLECharacteristic characteristic);

void updateChannel(BLEDevice central, BLECharacteristic characteristic);
