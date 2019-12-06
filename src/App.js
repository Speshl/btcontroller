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
    let deviceState = {
      selectedDevice: updates.server,
      connectionStatus: 2, //connected
      lightsOn: updates.data[0]
    }

    let primaryColorState = {
      colorList: ["#FFFFFF", "#FF0000", "#00FF00", "#0000FF", "#FFFF00", "#FF00FF", "#00FFFF"], //need to find a way to add selected color to list incase it is not already there
      isPickingColor: false,
      red: updates.data[2][0].primaryColor.red,
      green: updates.data[2][0].primaryColor.green,
      blue: updates.data[2][0].primaryColor.blue
    }

    let secondaryColorState = { //currently matches primary, need to add full secondary color functionality
      colorList: ["#FFFFFF", "#FF0000", "#00FF00", "#0000FF", "#FFFF00", "#FF00FF", "#00FFFF"], //need to find a way to add selected color to list incase it is not already there
      isPickingColor: false,
      red: updates.data[2][0].secondaryColor.red,
      green: updates.data[2][0].secondaryColor.green,
      blue: updates.data[2][0].secondaryColor.blue
    }

    let commandState = {
      animation: updates.data[2][0].animation,
      delay: updates.data[2][0].delay
    }

    let channelState = {
      selectedChannelList: updates.data[2][0].usedChannels,
      channelDescriptions: this.getChannelDescriptions(updates.data[1]),
      editedChannelDescription: null
    }

    this.setState({
      deviceState: deviceState,
      channelState: channelState,
      primaryColorState: primaryColorState,
      secondaryColorState: secondaryColorState,
      commandState: commandState
    });
  }

  getChannelDescriptions = (descriptions) => {
    let returnArray = [];
    for(let i=0; i<8; i++){
      let entry = null;
      if(descriptions != null && descriptions[i] != null){
        entry = {
          id: i,
          type: descriptions[i].stripData.type,
          numLEDs: descriptions[i].numLEDs,
          position: descriptions[i].position,
          order: descriptions[i].stripData.order
        }
      }else{
        entry = {
          id: i,
          type: null,
          numLEDs: null,
          position: null,
          order: null
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


