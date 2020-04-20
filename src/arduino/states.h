
#include <FlashStorage.h>

int getNumUseablePositions(state* currentState, int channelIndex);

bool getInteriorSwitchState();

bool getAlternateCommandSwitchState();

void mapPositions(dynamicState *dState);

int getLastRow(state* currentState);

int getLongestChannel(state* currentState, int rowIndex);

bool isChannelActive(state* currentState, int channelIndex);

void setInitialTempState(tempState *tState);

bool updateTempState(state *currentState);

void setInitialDynamicState(dynamicState *dState);

commandState getAlternateCommandState();

commandState getCommand(state* currentState);

void setInitialChannelState(dynamicState *dState);

void setInitialCommandState(dynamicState *dState);

state* getCurrentState();

void saveState(dynamicState dState);
