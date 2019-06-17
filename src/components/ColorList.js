import React, { Component } from 'react'
import PropTypes from 'prop-types';
import { SketchPicker } from 'react-color';
import { CirclePicker } from 'react-color';
import BlueToothCommands from './../shared/BlueToothCommands';
import './../css/ColorList.css';

var colorList = ["#FFFFFF", "#FF0000", "#00FF00", "#0000FF", "#FFFF00", "#FF00FF", "#00FFFF"];
var selectedColor = null;
var newColor = null;

class ColorList extends Component {
    addColor = (e) => {
        this.props.stateUpdaters.setPickColorState(true);
    }

    addColorToList = () => {
        colorList.push(newColor);
        this.props.stateUpdaters.setPickColorState(false);
    }

    cancelColor = () => {
        this.props.stateUpdaters.setPickColorState(false);
    }

    handleSelectColor = async (color, e) => {
        if(color.hex.toUpperCase() !== selectedColor){
            selectedColor = color.hex.toUpperCase();
            this.props.stateUpdaters.updateSelectedColor(selectedColor);
            if(this.props.state.displayMode === "Basic"){
                try{
                    await BlueToothCommands.writeToDevice(this.props.state.selectedDevice, [{color:selectedColor, animation: "Static", delay: "0"}]);
                }catch(e){
                    alert(e);
                }
            }
        }
    }

    handleNewColor = (color, e) => {
        newColor = color.hex.toUpperCase();
    }

    toggleCustomColorPicker = () => {
        if(this.props.state.isPickingColor){
            return (
                <div>
                    <SketchPicker presetColors={[]} disableAlpha={true} width="250px" onChangeComplete={this.handleNewColor}/>
                    <button onClick={this.addColorToList}>Add</button>
                    <button onClick={this.cancelColor}>Cancel</button>
                </div>
            )
        }else{
            return <button onClick={this.addColor} >Add Custom Color</button>
        }
    }

    applySelectedColorCSS = () => {
        let selectedElement = document.querySelector('[title="'+this.props.state.selectedColor+'"]');
        if(selectedElement){
            selectedElement.click();
        }
    }

    render() {
        return (
            <div className="colorListMainDiv">
                <h3>Step 1: Choose a Color</h3>
                <div className="circlePickerDiv">
                    <CirclePicker colors={colorList} width="295px" onChangeComplete={this.handleSelectColor}/>
                </div>
                {this.toggleCustomColorPicker()}
            </div>
        )
    }

    componentDidUpdate = () => {
        this.applySelectedColorCSS();
    }

    componentDidMount = () => {
        this.applySelectedColorCSS();
    }
}

ColorList.propTypes = {
    state: PropTypes.object.isRequired
  }

export default ColorList;