import React from 'react';
import DeviceSelector from './components/DeviceSelector';
import DisplayBuilder from './components/DisplayBuilder';

import './css/App.css';

class App extends React.Component {

  state = {
    deviceState: {
      selectedDevice: null,
      connectionStatus: 0, //0 is disconnected, 1 is connecting, 2 is connected
      lightsOn: true
    },
    channelState: {
      selectedChannelList: [false, false, false, false, false, false, false, false],
      channelDescriptions: null,
      editedChannelDescription: null
    },
    primaryColorState: {
      colorList: ["#FFFFFF", "#FF0000", "#00FF00", "#0000FF", "#FFFF00", "#FF00FF", "#00FFFF"],
      isPickingColor: false,
      red: 255,
      green: 0,
      blue: 255,
    },
    secondaryColorState: {
      colorList: ["#FFFFFF", "#FF0000", "#00FF00", "#0000FF", "#FFFF00", "#FF00FF", "#00FFFF"],
      isPickingColor: false,
      red: 255,
      green: 0,
      blue: 255,
    },
    commandState: {
      animation: 0,
      delay: 0,
    }
  }

  updateDeviceState = (selectedDevice, connectionStatus, lightsOn) =>{
    let state = {
      selectedDevice: selectedDevice,
      connectionStatus: connectionStatus,
      lightsOn: lightsOn
    }
    this.setState({deviceState: state});
  }

  updateChannelState = (selectedChannelList, channelDescriptions, editedChannelDescription) => {
    let state = {
      selectedChannelList: selectedChannelList,
      channelDescriptions: channelDescriptions,
      editedChannelDescription: editedChannelDescription
    }
    this.setState({channelState: state});
  }

  updatePrimaryColorState = (colorList, isPickingColor, red, green, blue) => {
    let state = {
      colorList: colorList,
      isPickingColor: isPickingColor,
      red: red,
      green: green,
      blue: blue
    }
    this.setState({primaryColorState: state});
  }

  updateSecondaryColorState = (colorList, isPickingColor, red, green, blue) => {
    let state = {
      colorList: colorList,
      isPickingColor: isPickingColor,
      red: red,
      green: green,
      blue: blue
    }
    this.setState({secondaryColorState: state});
  }

  updateCommandState = (animation, delay) => {
    let state = {
      animation: animation,
      delay: delay
    }
    this.setState({commandState: state});
  }

  updateAllStates = (updates) => {
    try{
      let deviceState = {
        selectedDevice: updates.server,
        connectionStatus: 2, //connected
        lightsOn: updates.data.lightsOn
      }

      let primaryColorState = {
        colorList: ["#FFFFFF", "#FF0000", "#00FF00", "#0000FF", "#FFFF00", "#FF00FF", "#00FFFF"], //need to find a way to add selected color to list incase it is not already there
        isPickingColor: false,
        red: updates.data.command.primaryRed,
        green: updates.data.command.primaryGreen,
        blue: updates.data.command.primaryBlue
      }

      let secondaryColorState = { //currently matches primary, need to add full secondary color functionality
        colorList: ["#FFFFFF", "#FF0000", "#00FF00", "#0000FF", "#FFFF00", "#FF00FF", "#00FFFF"], //need to find a way to add selected color to list incase it is not already there
        isPickingColor: false,
        red: updates.data.command.secondaryRed,
        green: updates.data.command.secondaryGreen,
        blue: updates.data.command.secondaryBlue
      }

      let commandState = {
        animation: updates.data.command.animation,
        delay: updates.data.command.stepDelay
      }

      let channelState = {
        selectedChannelList: this.getSelectedChannelList(updates.data.channels),
        channelDescriptions: this.getChannelDescriptions(updates.data.channels),
        editedChannelDescription: null
      }

      this.setState({
        deviceState: deviceState,
        channelState: channelState,
        primaryColorState: primaryColorState,
        secondaryColorState: secondaryColorState,
        commandState: commandState
      });
    }catch(e){
      alert(e);
    }
  }

  getSelectedChannelList = (descriptions) => {
    let returnArray = [];
    for(let i=0; i<8; i++){
      if(descriptions != null && descriptions[i] != null){
        if(descriptions[i].stripUsed === 1){
          returnArray.push(true);
        }else{
          returnArray.push(false);
        }
      }else{
        returnArray.push(false);
      }
    }
    return returnArray;
  }

  getPositionFromInt = (intPosition) => {
    let numCols = 3;
    let row = Math.floor(intPosition / numCols);
    let col = intPosition % numCols;
    return row.toString() +":"+col.toString()
  }

  getChannelDescriptions = (descriptions) => {
    let returnArray = [];
    for(let i=0; i<8; i++){
      let entry = null;
      if(descriptions != null && descriptions[i] != null){
        entry = {
          id: i,
          type: descriptions[i].stripType,
          numLEDs: descriptions[i].numLEDs,
          position: this.getPositionFromInt(descriptions[i].stripPosition),
          order: descriptions[i].stripOrder,
          used: descriptions[i].stripUsed
        }
      }else{
        entry = {
          id: i,
          type: null,
          numLEDs: null,
          position: null,
          order: null,
          used: false
        }
      }
      returnArray.push(entry);
    }
    return returnArray;
  }

  stateUpdaters = {
    updateDeviceState: this.updateDeviceState,
    updateChannelState: this.updateChannelState,
    updatePrimaryColorState: this.updatePrimaryColorState,
    updateSecondaryColorState: this.updateSecondaryColorState,
    updateCommandState: this.updateCommandState,
    updateAllStates: this.updateAllStates
  }

  showDisplayBuilder = () =>{
    if(this.state.deviceState.connectionStatus === 2){ //should check for 2 (connected), using null while in development
      return (
        <React.Fragment>
          <DisplayBuilder state={this.state} stateUpdaters={this.stateUpdaters}/>
        </React.Fragment>
      )
    }
  }

  render() {
    return (
      <React.Fragment>
        <DeviceSelector stateUpdaters={this.stateUpdaters} state={this.state} />
        {this.showDisplayBuilder()}
      </React.Fragment>
    );
  }
}

export default App;


