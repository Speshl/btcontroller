void setInitialStrips(state* currentState);

void clearStrips(staticState* sState);

void setStrip(state* currentState, int i);

void fillStrips(state* currentState, uint32_t color);

void showAllStrips(state* currentState);

Adafruit_NeoPixel* getStripDefinition(int channelIndex, int type, int order, int numLEDs);

bool setStripColorAtPosition(state* currentState, int row, int col, int led, uint32_t color)
