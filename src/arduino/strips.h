
void setInitialStrips(state* currentState);

void clearStrips(staticState* sState);

void setStrip(state* currentState, int i);

void fillStrips(state* currentState, uint32_t color);

void showAllStrips(state* currentState);

Adafruit_NeoPixel* getStripDefinition(int channelIndex, int type, int order, int numLEDs);

bool setStripColorAtPosition(state* currentState, int row, int col, int pos, uint32_t color);

bool setStripColorAtPositionAcrossColumns(state* currentState, int row, int pos, uint32_t color);

bool setStripColorAtPositionWithHeight(state* currentState, int channelIndex, uint32_t color);
