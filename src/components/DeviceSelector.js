import React, { Component } from 'react';
import PropTypes from 'prop-types';
import './../css/DeviceSelector.css';

class DeviceSelector extends Component {

  connect = async () => {
    try{
      console.log("Attempting Bluetooth Connection...");
      await this.props.state.bluetoothHandler.connect(this.onDisconnected);
      console.log("Bluetooth Connection Established.");
      console.log("Starting Bluetooth Read...");
      await this.props.state.bluetoothHandler.read();
      console.log("Finished Bluetooth Read.");
      this.props.stateUpdaters.update();
    }catch(e){
      if(e.message.includes("User cancelled the requestDevice() chooser") === false){
        alert(e);
      }
      this.props.stateUpdaters.update();
    }
  }

  setUIBusyForConnect = () => {
    this.props.state.uiBusy = true;
    this.props.state.doConnection = true;
    this.props.stateUpdaters.update();
  }

  disconnect = async () => {
    if(this.props.state.bluetoothHandler.isConnected()){
      this.props.state.bluetoothHandler.disconnect();
      this.props.stateUpdaters.update();
    }
  }

  onDisconnected = async () => {
    await this.disconnect()
  }

  read = async () => {
    if(this.props.state.bluetoothHandler.isAvailable()){
      try{
        console.log("Starting Bluetooth Read...");
        await this.props.state.bluetoothHandler.read();
        console.log("Finished Bluetooth Read.");
      }catch(e){
        this.onDisconnected();
        alert(e);
      }
      this.props.stateUpdaters.update();
    }
  }

  setUIBusyForRead = () => {
    this.props.state.uiBusy = true;
    this.props.state.doRead = true;
    this.props.stateUpdaters.update();
  }

  write = async () => {
    if(this.props.state.bluetoothHandler.isAvailable()){
      try{
        console.log("Starting Bluetooth Write...");
        await this.props.state.bluetoothHandler.write();
        console.log("Finished Bluetooth Write.");
        this.props.stateUpdaters.update();
      }catch(e){
        alert(e);
      }
    }
  }

  setUIBusyForWrite = () => {
    this.props.state.uiBusy = true;
    this.props.state.doWrite = true;
    this.props.stateUpdaters.update();
  }

/*  toggleLightStatus = async () => {
    if(this.props.state.deviceState.connectionStatus === 2){
      try{
        let status = await BlueToothCommands.toggleLights(this.props.state.deviceState.selectedDevice, this.props.state.deviceState.lightsOn);
        if(status){//success
          this.props.stateUpdaters.updateDeviceState(
            this.props.state.deviceState.selectedDevice,
            this.props.state.deviceState.connectionStatus,
            !this.props.state.deviceState.lightsOn,
            this.props.state.deviceState.interiorLightsOn
          );
        }
      }catch(e){
        alert(e);
      }
    }
  }*/

  /*toggleInteriorLightStatus = async () => {
    if(this.props.state.deviceState.connectionStatus === 2){
      try{
        let status = await BlueToothCommands.toggleInteriorLights(this.props.state.deviceState.selectedDevice, this.props.state.deviceState.interiorLightsOn);
        if(status){//success
          this.props.stateUpdaters.updateDeviceState(
            this.props.state.deviceState.selectedDevice,
            this.props.state.deviceState.connectionStatus,
            this.props.state.deviceState.lightsOn,
            !this.props.state.deviceState.interiorLightsOn
          );
        }
      }catch(e){
        alert(e);
      }
    }
  }*/

  getCurrentStatusBorderStyle = () => {
    if(this.props.state.bluetoothHandler.isAvailable()){
      return{
        borderColor: "green"
      }
    }
    if(this.props.state.bluetoothHandler.isConnected() || this.props.state.uiBusy){
      return{
        borderColor: "yellow"
      }
    }else{
      return{
        borderColor: "red"
      }
    }
  }

  getCurrentStatusTextStyle = () => {
    if(this.props.state.bluetoothHandler.isAvailable()){
      return{
        color: "green"
      }
    }
    if(this.props.state.bluetoothHandler.isConnected() || this.props.state.uiBusy){
      return{
        color: "yellow"
      }
    }else{
      return{
        color: "red"
      }
    }
  }

  getCurrentStatusText = () => {
    if(this.props.state.bluetoothHandler.isAvailable()){
      return "Status: Connected";
    }
    if(this.props.state.bluetoothHandler.isConnected() || this.props.state.uiBusy){
      return "Status: Busy...";
    }else{
      return "Disconnected";
    }
  }

  /*getInteriorLightsToggleButton = () => {
    if(this.props.state.deviceState.connectionStatus === 2){
      console.log(this.props.state.deviceState.interiorLightsOn)
      if(this.props.state.deviceState.interiorLightsOn){
        return <div><button className="lightsToggle" onClick={this.toggleInteriorLightStatus}>Toggle Interior Lights Off</button></div>
      }else{
        return <div><button className="lightsToggle" onClick={this.toggleInteriorLightStatus}>Toggle Interior Lights On</button></div>
      }
    }
  }*/

  /*getLightsToggleButton = () => {
    if(this.props.state.deviceState.connectionStatus === 2){
      if(this.props.state.deviceState.lightsOn){
        return <div><button className="lightsToggle" onClick={this.toggleLightStatus}>Toggle Lights Off</button></div>
      }else{
        return <div><button className="lightsToggle" onClick={this.toggleLightStatus}>Toggle Lights On</button></div>
      }
    }
  }*/

  getLoadButton = () => {
    if(this.props.state.bluetoothHandler.isAvailable()){
      return <React.Fragment>
        <button className="saveButton" onClick={this.setUIBusyForRead}>Load Settings</button>
      </React.Fragment>
    }
  }

  getSaveButton = () => {
    if(this.props.state.bluetoothHandler.isAvailable()){
      return <React.Fragment>
        <button className="saveButton" onClick={this.setUIBusyForWrite}>Save Settings</button>
      </React.Fragment>
    }
  }

  render() {
    return (
      <div className="selectorMainDiv" style = {this.getCurrentStatusBorderStyle()}>
        <h2>Device Selector</h2>
        <button className="selectorButton" onClick={this.setUIBusyForConnect}>Connect Device</button>
        <button className="selectorButton" onClick={this.disconnect}>Disconnect Device</button>
        <div className="selectorLabelDiv">
          <div>
            <label className="selectorLabel">Name: {(this.props.state.bluetoothHandler.server) ? this.props.state.bluetoothHandler.server.device.name : ""}</label>
          </div>
          <div>
            <label className="selectorLabel" style={this.getCurrentStatusTextStyle()}>{this.getCurrentStatusText()}</label>
          </div>
          <div>
            {this.getLoadButton()}
            {this.getSaveButton()}
          </div>
          {/*this.getLightsToggleButton()}
          {this.getInteriorLightsToggleButton()*/}
        </div>
      </div>
    );
  }

  componentDidUpdate = () => {
    if(this.props.state.uiBusy){
      this.props.state.uiBusy = false;
      
      if(this.props.state.doConnection){
        this.props.state.doConnection = false;
        this.connect();
      }else if(this.props.state.doRead){
        this.props.state.doRead = false;
        this.read();
      }else if(this.props.state.doWrite){
        this.props.state.doWrite = false;
        this.write();
      }
    }
  }  
}
DeviceSelector.propTypes = {
  state: PropTypes.object.isRequired
}

export default DeviceSelector;
