#include <SoftwareSerial.h>
#include <EEPROM.h>

#define REDPIN 6 //PWM
#define GREENPIN 9 //PWM
#define BLUEPIN 10 //PWM
#define WHITEPIN 11 //PWM NEED TO CHECK PIN

struct command
{
  char animation;
  int red;
  int green;
  int blue;
  int delay;
};

typedef struct command Command;

bool debug = true;
bool lightStatus = false;

SoftwareSerial btSerial(2, 3);
// Connect HM10      Arduino Uno
//     Pin 1/TXD          Pin 2
//     Pin 2/RXD          Pin 3
const int NUMCOMMANDS = 10;
const int NUMCOMMANDATTRIBUTES = 5;
const int MAXCOMMANDSIZE = 5;
int attributeIndex = 0;
int commandIndex = 0;
int saveIndex = 0;
int currentLightShowIndex = 0;
bool newCommand = false;
char newCommands [NUMCOMMANDS] [NUMCOMMANDATTRIBUTES] [MAXCOMMANDSIZE];
Command commands [NUMCOMMANDS];

int currentDisplayedRed = 0;
int currentDisplayedGreen = 0;
int currentDisplayedBlue = 0;

void setup() {
  readEEPROM();
  
  digitalWrite(REDPIN, LOW);
  digitalWrite(GREENPIN, LOW);
  digitalWrite(BLUEPIN, LOW);
  digitalWrite(WHITEPIN, LOW);

  Serial.begin(9600);
  btSerial.begin(9600);
  debugMessage("Ready.");
}

void loop() {
  if (btSerial.available() > 0) {
    char temp = btSerial.read();
    if (temp == '?') {
      debugMessage("Found ?");
      sendStatus();
    } else if (temp == '(') {
      debugMessage("Found (");
      updateLightStatus(true);
    } else if (temp == ')') {
      debugMessage("Found )");
      updateLightStatus(false);
    } else if (temp == '[') {
      memset(newCommands, 0, sizeof(newCommands));
      debugMessage("Found Color Command Start Character");
      commandIndex = 0;
      attributeIndex = 0;
      saveIndex = 0;
    } else if (temp == ']') {
      debugMessage("Found Color Command End Character");
      newCommand = true;
    } else if (temp == '|') {
      debugMessage("Found Command Delimiter Character");
      commandIndex++;
      attributeIndex = 0;
      saveIndex = 0;
    } else if (temp == ',') {
      debugMessage("Found Attribute Delimiter Character");
      attributeIndex++;
      saveIndex = 0;
    } else {
      newCommands[commandIndex][attributeIndex][saveIndex] = temp;
      saveIndex++;
    }
  }

  if (newCommand) {
    printNewCommands();
    updateCommands();
    updateEEPROM();
    printCommands();
    newCommand = false;
    currentLightShowIndex = 0;
  }

  runCommand();
}

void runCommand() {
  switch (commands[currentLightShowIndex].animation) {
    case 'S':
      runStaticCommand();
      break;
    case 'B':
      runBlinkCommand();
      break;
    case 'P':
      runPulseCommand();
      break;
    case 'F':
      runFadeCommand();
      break;
    default:
      debugMessage("Command doesn't have valid animation");
      break;
  }
}

void incrementLightShowIndex() {
  currentLightShowIndex++;
  if (currentLightShowIndex >= commandIndex) {
    currentLightShowIndex = 0;
  }
}

void runStaticCommand() {
  updateLights(commands[currentLightShowIndex].red, commands[currentLightShowIndex].green, commands[currentLightShowIndex].blue);
  delay(commands[currentLightShowIndex].delay);
  incrementLightShowIndex();
}
void runBlinkCommand() {
  updateLights(commands[currentLightShowIndex].red, commands[currentLightShowIndex].green, commands[currentLightShowIndex].blue);
  delay(commands[currentLightShowIndex].delay / 2);
  stopLights();
  delay(commands[currentLightShowIndex].delay / 2);
  incrementLightShowIndex();
}

void runPulseCommand() {
  stopLights();

  int totalDelay = commands[currentLightShowIndex].delay / 2;
  if (totalDelay < 255) {
    totalDelay = 255;
  }

  int currentRed = 0;
  int currentGreen = 0;
  int currentBlue = 0;

  int redCurrentDelay = 0;
  int greenCurrentDelay = 0;
  int blueCurrentDelay = 0;

  int redDelayPerStep = 0;
  int greenDelayPerStep = 0;
  int blueDelayPerStep = 0;

  if (commands[currentLightShowIndex].red > 0) {
    redDelayPerStep = totalDelay / commands[currentLightShowIndex].red;
  }

  if (commands[currentLightShowIndex].green > 0) {
    greenDelayPerStep = totalDelay / commands[currentLightShowIndex].green;
  }

  if (commands[currentLightShowIndex].blue > 0) {
    blueDelayPerStep = totalDelay / commands[currentLightShowIndex].blue;
  }

  //fade color up
  for (int i = 0; i < totalDelay; i++) {
    if (redDelayPerStep != 0 && redCurrentDelay == redDelayPerStep) {
      redCurrentDelay = 0;
      currentRed++;
      if (currentRed > 255) {
        currentRed = 255;
      }
    }

    if (greenDelayPerStep != 0 && greenCurrentDelay == greenDelayPerStep) {
      greenCurrentDelay = 0;
      currentGreen++;
      if (currentGreen > 255) {
        currentGreen = 255;
      }
    }

    if (blueDelayPerStep != 0 && blueCurrentDelay == blueDelayPerStep) {
      blueCurrentDelay = 0;
      currentBlue++;
      if (currentBlue > 255) {
        currentBlue = 255;
      }
    }

    updateLights(currentRed, currentGreen, currentBlue);
    redCurrentDelay++;
    greenCurrentDelay++;
    blueCurrentDelay++;
    delay(1);
  }
  //fade color out
  for (int i = 0; i < totalDelay; i++) {
    if (redDelayPerStep != 0 && redCurrentDelay == redDelayPerStep) {
      redCurrentDelay = 0;
      currentRed--;
      if (currentRed < 0) {
        currentRed = 0;
      }
    }

    if (greenDelayPerStep != 0 && greenCurrentDelay == greenDelayPerStep) {
      greenCurrentDelay = 0;
      currentGreen--;
      if (currentGreen < 0) {
        currentGreen = 0;
      }
    }

    if (blueDelayPerStep != 0 && blueCurrentDelay == blueDelayPerStep) {
      blueCurrentDelay = 0;
      currentBlue--;
      if (currentBlue < 0) {
        currentBlue = 0;
      }
    }

    updateLights(currentRed, currentGreen, currentBlue);
    redCurrentDelay++;
    greenCurrentDelay++;
    blueCurrentDelay++;
    delay(1);
  }
  stopLights();//make sure fade went all the way out
  incrementLightShowIndex();
}
//need to update to start from current color
void runFadeCommand() {
  int totalDelay = commands[currentLightShowIndex].delay;
  if (totalDelay < 255) {
    totalDelay = 255;
  }

  int currentRed = currentDisplayedRed;
  int currentGreen = currentDisplayedGreen;
  int currentBlue = currentDisplayedBlue;

  int redCurrentDelay = 0;
  int greenCurrentDelay = 0;
  int blueCurrentDelay = 0;

  int redDelayPerStep = 0;
  int redDifference = currentRed - commands[currentLightShowIndex].red; //get difference from where we are to where we are going
  bool redStepUp = false;//are we going up or down to make new color

  int greenDelayPerStep = 0;
  int greenDifference = currentGreen - commands[currentLightShowIndex].green; //get difference from where we are to where we are going
  bool greenStepUp = false;//are we going up or down to make new color

  int blueDelayPerStep = 0;
  int blueDifference = currentBlue - commands[currentLightShowIndex].blue; //get difference from where we are to where we are going
  bool blueStepUp = false;//are we going up or down to make new color
  
  if(redDifference < 0){
    redDifference = redDifference * - 1;
    redStepUp = true;
  }
  
  if(greenDifference < 0){
    greenDifference = greenDifference * - 1;
    greenStepUp = true;
  }
  
  if(blueDifference < 0){
    blueDifference = blueDifference * - 1;
    blueStepUp = true;
  }

  if (redDifference> 0) {
    redDelayPerStep = totalDelay / redDifference;
  }

  if (greenDifference > 0) {
    greenDelayPerStep = totalDelay / greenDifference;
  }

  if (blueDifference > 0) {
    blueDelayPerStep = totalDelay / blueDifference;
  }

  for (int i = 0; i < totalDelay; i++) {
    if (redDelayPerStep != 0 && redCurrentDelay == redDelayPerStep) {
      redCurrentDelay = 0;
      if(redStepUp){
        currentRed++;
      }else{
        currentRed--;
      }
      if(currentRed > 255) {
        currentRed = 255;
      }else if(currentRed < 0){
        currentRed = 0;
      }
    }

    if (greenDelayPerStep != 0 && greenCurrentDelay == greenDelayPerStep) {
      greenCurrentDelay = 0;
      if(greenStepUp){
        currentGreen++;
      }else{
        currentGreen--;
      }
      if(currentGreen > 255) {
        currentGreen = 255;
      }else if(currentGreen < 0){
        currentGreen = 0;
      }
    }

    if (blueDelayPerStep != 0 && blueCurrentDelay == blueDelayPerStep) {
      blueCurrentDelay = 0;
      if(blueStepUp){
        currentBlue++;
      }else{
        currentBlue--;
      }
      if(currentBlue > 255) {
        currentBlue = 255;
      }else if(currentBlue < 0){
        currentBlue = 0;
      }
    }

    updateLights(currentRed, currentGreen, currentBlue);
    redCurrentDelay++;
    greenCurrentDelay++;
    blueCurrentDelay++;
    delay(1);
  }

  incrementLightShowIndex();
}

void updateLights(int red, int green, int blue) {
  currentDisplayedRed = red;
  currentDisplayedGreen = green;
  currentDisplayedBlue = blue;
  if (red == 255 && blue == 255 && green == 255) {
    stopLights();
    analogWrite(WHITEPIN, 255);
  } else {
    analogWrite(REDPIN, red);
    analogWrite(GREENPIN, green);
    analogWrite(BLUEPIN, blue);
  }
}

void stopLights() {
  currentDisplayedRed = 0;
  currentDisplayedGreen = 0;
  currentDisplayedBlue = 0;
  analogWrite(REDPIN, 0);
  analogWrite(GREENPIN, 0);
  analogWrite(BLUEPIN, 0);
  analogWrite(WHITEPIN, 0);
}

void printNewCommands() {
  for (int i = 0; i <= commandIndex; i++) {
    for (int j = 0; j <= attributeIndex; j++) {
      Serial.println(newCommands[i][j]);
    }
  }
}

void printCommands() {
  for (int i = 0; i < commandIndex; i++) {
    Serial.print(commands[i].red);
    Serial.print(",");
    Serial.print(commands[i].green);
    Serial.print(",");
    Serial.print(commands[i].blue);
    Serial.print(",");
    Serial.print(commands[i].animation);
    Serial.print(",");
    Serial.print(commands[i].delay);
    Serial.print("\r\n");
  }
}

void updateEEPROM(){
  int eeAddress = 0;
  EEPROM.update(eeAddress, lightStatus);
  eeAddress += sizeof(lightStatus);
  EEPROM.update(eeAddress,commandIndex);
  eeAddress += sizeof(commandIndex);
  EEPROM.update(eeAddress, commands);
}

void readEEPROM(){
  int eeAddress = 0;
  EEPROM.get(eeAddress, lightStatus);
  eeAddress += sizeof(lightStatus);
  EEPROM.get(eeAddress,commandIndex);
  eeAddress += sizeof(commandIndex);
  EEPROM.get(eeAddress, commands);
}

void updateCommands() {
  for (int i = 0; i <= commandIndex; i++) {
    for (int j = 0; j <= attributeIndex; j++) {
      switch (j) {
        case 0:
          commands[i].red = atoi(newCommands[i][j]);
          break;
        case 1:
          commands[i].green = atoi(newCommands[i][j]);
          break;
        case 2:
          commands[i].blue = atoi(newCommands[i][j]);
          break;
        case 3:
          commands[i].animation = newCommands[i][j][0];
          break;
        case 4:
          commands[i].delay = atoi(newCommands[i][j]);
          break;
      }
    }
  }
}

void sendStatus() {
  debugMessage("Sending Status");
  btSerial.print(boolToString(lightStatus));
}

void updateLightStatus(bool status) {
  //debugMessage("Updating Light status: "+boolToString(status));
  lightStatus = status;
  sendStatus();
}

char * boolToString(bool value) {
  if (value) {
    return "[true]";
  } else {
    return "[false]";
  }
}

void debugMessage(char m[]) {
  if (debug) {
    Serial.println(m);
  }
}
