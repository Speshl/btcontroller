import React, { Component } from 'react';
import PropTypes from 'prop-types';
import BlueToothCommands from './../shared/BlueToothCommands';
import './../css/DeviceSelector.css';

//this.props.selectedDevice
class DeviceSelector extends Component {
  connect = async () => {
    try{
      let device = await BlueToothCommands.connect(this.onDisconnected, this.onDataRecieved);
      this.props.stateUpdaters.updateSelectedDevice(device);
      return;
    }catch(e){
      alert(e);
    }
  }

  disconnect = async () => {
    if(this.props.state.selectedDevice){
      BlueToothCommands.disconnect(this.props.state.selectedDevice);
    }
  }

  onDisconnected = () => {
    this.props.stateUpdaters.updateSelectedDevice(null);
  }

  onDataRecieved = (event) => {
    let decodedValue = (new TextDecoder().decode(event.target.value)).trim();
    if(decodedValue === "[true]"){
      this.props.stateUpdaters.toggleOn(true);
    }else if(decodedValue === '[false]'){
      this.props.stateUpdaters.toggleOn(false);
    }
  }

  getCurrentStatusBorderStyle = () => {
    return{
      borderColor: (BlueToothCommands.isConnected(this.props.state.selectedDevice)) ? "green" : "red"
    }
  }

  getCurrentStatusTextStyle = () => {
    return{
      color: (BlueToothCommands.isConnected(this.props.state.selectedDevice)) ? "green" : "red"
    }
  }

  getCurrentStatusText = () => {
    let status = "Disconnected";
    if(BlueToothCommands.isConnected(this.props.state.selectedDevice)){
      status = "Connected";
    }
    return status;
  }

  toggleLightStatus = () => {
    if(BlueToothCommands.isConnected(this.props.state.selectedDevice)){
      if(this.props.state.lightsOn){
        BlueToothCommands.sendBTCommand(this.props.state.selectedDevice, ')');//Send off command
      }else{
        BlueToothCommands.sendBTCommand(this.props.state.selectedDevice, '(');//Send on command
      }
    }
  }

  getLightsToggleButton = () => {
    if(BlueToothCommands.isConnected(this.props.state.selectedDevice)){
      if(this.props.state.lightsOn){
        return <div><button className="lightsToggle" onClick={this.toggleLightStatus}>Toggle Lights Off</button></div>
      }else{
        return <div><button className="lightsToggle" onClick={this.toggleLightStatus}>Toggle Lights On</button></div>
      }
    }
  }

  render() {
    return (
      <div className="selectorMainDiv" style = {this.getCurrentStatusBorderStyle()}>
        <h2>Device Selector</h2>
        <button className="selectorButton" onClick={this.connect}>Connect Device</button>
        <button className="selectorButton" onClick={this.disconnect}>Disconnect Device</button>
        <div className="selectorLabelDiv">
          <div>
            <label className="selectorLabel">Name: {(this.props.state.selectedDevice) ? this.props.state.selectedDevice.name : ""}</label>
          </div>
          <div>
            <label className="selectorLabel" style={this.getCurrentStatusTextStyle()}>Status: {this.getCurrentStatusText()}</label>
          </div>
          {this.getLightsToggleButton()}
        </div>
      </div>
    );
  }
}

DeviceSelector.propTypes = {
  state: PropTypes.object.isRequired
}

export default DeviceSelector;
