import React, { Component } from 'react';
import PropTypes from 'prop-types';
import BlueToothCommands from './../shared/BlueToothCommands';
import './../css/DeviceSelector.css';

//this.props.selectedDevice
class DeviceSelector extends Component {

  connect = async () => {
    try{
      let device = await BlueToothCommands.connect(this.onDisconnected);
      this.props.stateUpdaters.updateDeviceState(device, 1, true);
    }catch(e){
      if(e.message.includes("User cancelled the requestDevice() chooser") === false){
        alert(e);
      }
    }
  }

  disconnect = async () => {
    if(this.props.state.deviceState.connectionStatus === 2){
      BlueToothCommands.disconnect(this.props.state.deviceState.selectedDevice);
    }
  }

  onDisconnected = () => {
    this.props.stateUpdaters.updateDeviceState(null, 0, true);
  }

  load = async () => {
    if(this.props.state.deviceState.connectionStatus === 1){
      let data = await BlueToothCommands.load(this.props.state.deviceState.selectedDevice);
      this.props.stateUpdaters.updateAllStates({server: this.props.state.deviceState.selectedDevice, data: data});
    }
  }

  save = async () => {
    if(this.props.state.selectedDevice){
      BlueToothCommands.save(this.props.state.deviceState.selectedDevice);
    }
  }

  toggleLightStatus = async () => {
    if(this.props.state.deviceState.connectionStatus === 2){
        let status = await BlueToothCommands.toggleLights(this.props.state.deviceState.selectedDevice, this.props.state.deviceState.lightsOn);
        if(status){//success
          this.props.stateUpdaters.updateDeviceState(
            this.props.state.deviceState.selectedDevice,
            this.props.state.deviceState.connectionStatus,
            !this.props.state.deviceState.lightsOn
          );
        }
    }
  }

  getCurrentStatusBorderStyle = () => {
    if(this.props.state.deviceState.connectionStatus === 0){
      return{
        borderColor: "red"
      }
    }else if(this.props.state.deviceState.connectionStatus === 1){
      return{
        borderColor: "yellow"
      }
    }else if(this.props.state.deviceState.connectionStatus === 2){
      return{
        borderColor: "green"
      }
    }
  }

  getCurrentStatusTextStyle = () => {
    if(this.props.state.deviceState.connectionStatus === 0){
      return{
        color: "red"
      }
    }else if(this.props.state.deviceState.connectionStatus === 1){
      return{
        color: "yellow"
      }
    }else if(this.props.state.deviceState.connectionStatus === 2){
      return{
        color: "green"
      }
    }
  }

  getCurrentStatusText = () => {
    let status = "Status: ";
    if(this.props.state.deviceState.connectionStatus === 0){
      status += "Disconnected";
    }else if(this.props.state.deviceState.connectionStatus === 1){
      status += "Loading saved data...";
    }else if(this.props.state.deviceState.connectionStatus === 2){
      status += "Connected";
    }
    return status;
  }

  getLightsToggleButton = () => {
    if(this.props.state.deviceState.connectionStatus === 2){
      if(this.props.state.deviceState.lightsOn){
        return <div><button className="lightsToggle" onClick={this.toggleLightStatus}>Toggle Lights Off</button></div>
      }else{
        return <div><button className="lightsToggle" onClick={this.toggleLightStatus}>Toggle Lights On</button></div>
      }
    }
  }

  getSaveButton = () => {
    if(this.props.state.deviceState.connectionStatus === 2){
      return <React.Fragment>
        <button className="saveButton" onClick={this.save}>Save CommandSet</button>
      </React.Fragment>
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
            <label className="selectorLabel">Name: {(this.props.state.deviceState.selectedDevice) ? this.props.state.deviceState.selectedDevice.device.name : ""}</label>
          </div>
          <div>
            <label className="selectorLabel" style={this.getCurrentStatusTextStyle()}>{this.getCurrentStatusText()}</label>
          </div>
          <div>
            {this.getSaveButton()}
          </div>
          {this.getLightsToggleButton()}
        </div>
      </div>
    );
  }

  componentDidUpdate = () => {
    if(this.props.state.deviceState.connectionStatus === 1){
      this.load();
    }
  }  
}
DeviceSelector.propTypes = {
  state: PropTypes.object.isRequired
}

export default DeviceSelector;
