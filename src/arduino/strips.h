
void setInitialStrips(state* currentState);

void clearStrips(staticState* sState);

void setStrip(state* currentState, int i);

void fillStrips(state* currentState, uint32_t color);

void showAllStrips(state* currentState);

Adafruit_NeoPixel* getStripDefinition(int channelIndex, int type, int order, int numLEDs);

bool setStripColorAtLocation(state* currentState, int row, int col, uint32_t color);

bool setStripColorAtPosition(state* currentState, int row, int col, int pos, uint32_t color);

bool setStripColorAtPositionAcrossColumns(state* currentState, int row, int pos, uint32_t color);

bool setStripColorAtPositionWithHeight(state* currentState, int channelIndex, uint32_t color);

uint32_t getColorOfChannelAtPosition(state* currentState, int channelIndex, int pos);

void setColorOfChannelAtPosition(state* currentState, int channelIndex, int pos, uint32_t color);

void saveStripState(state* currentState);

void restoreStripState(state* currentState);
