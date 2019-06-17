import React from 'react';
import DeviceSelector from './components/DeviceSelector';
import DisplayBuilder from './components/DisplayBuilder';
import './css/App.css';

class App extends React.Component {

  state = {
    selectedDevice: null,
    lightsOn: true,
    displayMode: "Basic",
    isPickingColor: false,
    selectedColor: "#FFFFFF",
    selectedAnimation: "Static",
    selectedDelay: "100",
    selectedCommand: null,
    commandList: []
  }

  updateSelectedDevice = (device) => {
    this.setState({selectedDevice: device});
  }

  toggleOn = (status) => {
    if(status === true || status === false){
      this.setState({lightsOn: status});
    }else{
      if(this.state.lightsOn){
        this.setState({lightsOn: false});
      }else{
        this.setState({lightsOn: true});
      }
    }
  }

  toggleDisplayMode= () => {
    if(this.state.displayMode === "Basic"){
      this.setState({displayMode: "Advanced"});
    }else{
      this.setState({displayMode: "Basic", selectedAnimation: "Static", commandList: [], selectedCommand: null});
    }
  }

  setPickColorState = (state) => {
    this.setState({isPickingColor: state});
  }

  updateSelectedColor = (color) => {
    this.setState({selectedColor: color});
  }

  updateSelectedAnimation = (animation) => {
    this.setState({selectedAnimation: animation});
  }

  updateSelectedDelay = (delay) => {
    this.setState({selectedDelay: delay});
  }

  updateSelectedCommand = (commandIndex) => {
    this.setState({selectedCommand: commandIndex});
  }

  updateCommandList = (updatedCommandList) => {
    this.setState({commandList: updatedCommandList})
  }

  showDisplayBuilder = () =>{
    if(this.state.selectedDevice || true){
      return <DisplayBuilder state={this.state} stateUpdaters={this.stateUpdaters} />
    }
  }

  stateUpdaters = {
    updateSelectedDevice: this.updateSelectedDevice,
    toggleOn: this.toggleOn,
    toggleDisplayMode: this.toggleDisplayMode,
    setPickColorState: this.setPickColorState,
    updateSelectedColor: this.updateSelectedColor,
    updateSelectedAnimation: this.updateSelectedAnimation,
    updateSelectedDelay: this.updateSelectedDelay,
    updateSelectedCommand: this.updateSelectedCommand,
    updateCommandList: this.updateCommandList
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
