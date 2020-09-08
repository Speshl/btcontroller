import React from 'react';
import DeviceSelector from './components/DeviceSelector';
import DisplayBuilder from './components/DisplayBuilder';
import BluetoothHandler from './classes/BluetoothHandler';

import './css/App.css';
import Channel from './classes/Channel';

class App extends React.Component {


  state = {
    uiBusy: false,
    doConnection: false,
    doRead: false,
    doWrite: false,
    bluetoothHandler: new BluetoothHandler(16),
    editedChannel: new Channel(null),
    primaryColorState: {
      colorList: ["#FFFFFF", "#FF0000", "#00FF00", "#0000FF", "#FFFF00", "#FF00FF", "#00FFFF"],
      isPickingColor: false
    },
    secondaryColorState: {
      colorList: ["#FFFFFF", "#FF0000", "#00FF00", "#0000FF", "#FFFF00", "#FF00FF", "#00FFFF"],
      isPickingColor: false
    }
  }

  updatePrimaryColorState = (colorList, isPickingColor, red, green, blue) => {
    let state = {
      colorList: colorList,
      isPickingColor: isPickingColor
    }
    this.setState({primaryColorState: state});
  }

  updateSecondaryColorState = (colorList, isPickingColor, red, green, blue) => {
    let state = {
      colorList: colorList,
      isPickingColor: isPickingColor
    }
    this.setState({secondaryColorState: state});
  }

  updateAllStates = (updates) => {
    try{

      let primaryColorState = {
        colorList: ["#FFFFFF", "#FF0000", "#00FF00", "#0000FF", "#FFFF00", "#FF00FF", "#00FFFF"], //need to find a way to add selected color to list incase it is not already there
        isPickingColor: false
      }

      let secondaryColorState = { //currently matches primary, need to add full secondary color functionality
        colorList: ["#FFFFFF", "#FF0000", "#00FF00", "#0000FF", "#FFFF00", "#FF00FF", "#00FFFF"], //need to find a way to add selected color to list incase it is not already there
        isPickingColor: false
      }

      this.setState({
        primaryColorState: primaryColorState,
        secondaryColorState: secondaryColorState
      });
    }catch(e){
      alert(e);
    }
  }

  updateScreen = () => {
    this.setState(this.state);
  }

  getPositionFromInt = (intPosition) => {
    let numCols = 3;
    let row = Math.floor(intPosition / numCols);
    let col = intPosition % numCols;
    return row.toString() +":"+col.toString()
  }

  stateUpdaters = {
    updatePrimaryColorState: this.updatePrimaryColorState,
    updateSecondaryColorState: this.updateSecondaryColorState,
    updateAllStates: this.updateAllStates,
    update: this.updateScreen
  }

  showDisplayBuilder = () =>{
    console.log("Showing display builder");
    if(this.state.bluetoothHandler.isAvailable()){ //should check for 2 (connected), using 0 while in development
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


