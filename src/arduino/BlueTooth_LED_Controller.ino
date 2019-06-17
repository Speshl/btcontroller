#include <SoftwareSerial.h>

bool debug = true;
bool lightStatus = false;

SoftwareSerial btSerial(2, 3); // RX, TX  
// Connect HM10      Arduino Uno
//     Pin 1/TXD          Pin 2
//     Pin 2/RXD          Pin 3
const int NUMCOMMANDS = 6;
String newCommands [NUMCOMMANDS];
String commands [NUMCOMMANDS];
int index = 0;
bool newCommand = false;

void setup() {  
  Serial.begin(9600);
  // If the baudrate of the HM-10 module has been updated,
  // you may need to change 9600 by another value
  // Once you have found the correct baudrate,
  // you can update it using AT+BAUDx command 
  // e.g. AT+BAUD0 for 9600 bauds
  btSerial.begin(9600);
  //btSerial.println("["+String(lightStatus)+"]");
  debugMessage("Ready.");
}

void loop() {  
if(btSerial.available() > 0){
    char temp = btSerial.read();
    debugMessage("Read: "+ String(temp));
    if(temp == '?'){
      sendStatus();
    }else if(temp == '('){
      updateLightStatus(true);
    }else if(temp == ')'){
      updateLightStatus(false);
    }else if(temp == '['){
      debugMessage("Found Color Command Start Character");
      memset(newCommands, 0, sizeof(newCommands));
      index = 0;
    }else if(temp == ']'){
      debugMessage("Found Color Command End Character");
      newCommand = true;
    }else if(temp == '|'){
      debugMessage("Full Command: "+newCommands[index]);
      debugMessage("Found Delimiter Character");
      index++;
    }else{
      newCommands[index] += String(temp);
    }
  }
  
  if(newCommand){
    applyNewCommands();
    debugMessage("New Command Found: "+ getCommandsText());
    newCommand = false;
  }
}

void updateLightStatus(bool status){
  debugMessage("Updating Light status: "+boolToString(status));
  lightStatus = status;
  sendStatus();
}

void sendStatus(){
  debugMessage("Sending Status");
  btSerial.print("["+ boolToString(lightStatus) +"]");
}

String boolToString(bool value){
  if(value){
    return "true";
  }else{
    return "false";
  }
}

void applyNewCommands(){
  debugMessage("Applying new commands");
  for(int i=0; i<NUMCOMMANDS; i++){
    commands[i] = newCommands[i];
  }
}

String getCommandsText() {
  String returnText = "";
  for(int i=0; i<NUMCOMMANDS; i++){
    returnText += String(i) + ": " + commands[i] + "\r\n";
  }
  return returnText;
}

void debugMessage(String m){
  if(debug){
    Serial.println(m); 
  }
}
