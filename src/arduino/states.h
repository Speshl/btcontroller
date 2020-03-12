#include <FlashStorage.h>

bool getInteriorSwitchState();

void mapPositions(dynamicState *dState);

int getLastRow(state* currentState);

int getLongestChannel(state* currentState, int rowIndex);

bool isChannelActive(state* currentState, int channelIndex);

void setInitialTempState(tempState *tState);

void setInitialDynamicState(dynamicState *dState);

void setInitialChannelState(dynamicState *dState);

void setInitialCommandState(dynamicState *dState);

state* getCurrentState();

void saveState(dynamicState dState);
