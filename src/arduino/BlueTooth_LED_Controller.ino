#include <SoftwareSerial.h>

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
bool newCommand = false;
char newCommands [NUMCOMMANDS] [NUMCOMMANDATTRIBUTES] [MAXCOMMANDSIZE];
Command commands [NUMCOMMANDS];

void setup() {  
  Serial.begin(9600);
  btSerial.begin(9600);
  debugMessage("Ready.");
}

void loop() {  
  if(btSerial.available() > 0){
    char temp = btSerial.read();
    if(temp == '?'){
      debugMessage("Found ?");
      sendStatus();
    }else if(temp == '('){
      debugMessage("Found (");
      updateLightStatus(true);
    }else if(temp == ')'){
      debugMessage("Found )");
      updateLightStatus(false);
    }else if(temp == '['){
      memset(newCommands, 0, sizeof(newCommands));
      debugMessage("Found Color Command Start Character");
      commandIndex = 0;
      attributeIndex = 0;
      saveIndex = 0;
    }else if(temp == ']'){
      debugMessage("Found Color Command End Character");
      newCommand = true;
    }else if(temp == '|'){
      debugMessage("Found Command Delimiter Character");
      commandIndex++;
      attributeIndex = 0;
      saveIndex = 0;
    }else if(temp == ','){
      debugMessage("Found Attribute Delimiter Character");
      attributeIndex++;
      saveIndex = 0;
    }else{
      newCommands[commandIndex][attributeIndex][saveIndex] = temp;
      saveIndex++;
    }
  }
  
  if(newCommand){
    printNewCommands();
    updateCommands();
    printCommands();
    newCommand = false;
  }
}

void printNewCommands(){
  for(int i=0; i<=commandIndex; i++){
    for(int j=0; j<=attributeIndex; j++){
      Serial.println(newCommands[i][j]);
    }
  }
}

void printCommands(){
  for(int i=0; i<commandIndex; i++){
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

void updateCommands(){
  for(int i=0; i<=commandIndex; i++){
    for(int j=0; j<=attributeIndex; j++){
      switch(j){
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

void sendStatus(){
  debugMessage("Sending Status");
  btSerial.print(boolToString(lightStatus));
}

void updateLightStatus(bool status){
  //debugMessage("Updating Light status: "+boolToString(status));
  lightStatus = status;
  sendStatus();
}

char * boolToString(bool value){
  if(value){
    return "[true]";
  }else{
    return "[false]";
  }
}

void debugMessage(char m[]){
  if(debug){
    Serial.println(m); 
  }
}
