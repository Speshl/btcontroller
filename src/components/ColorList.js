import React, { Component } from 'react'
import { SketchPicker } from 'react-color';
import { CirclePicker } from 'react-color';
import './../css/ColorList.css';

var newColor = "$FF00FF";

class ColorList extends Component {

    handleNewColor = (color, e) => {
        newColor = color.hex.toUpperCase();
        this.props.stateUpdaters.update();
    }

    addColor = (e) => {
        if(this.props.type === "primary"){
            let state = this.props.state.primaryColorState;
            this.props.stateUpdaters.updatePrimaryColorState(
                state.colorList,
                true
            );
        }else{
            let state = this.props.state.secondaryColorState;
            this.props.stateUpdaters.updateSecondaryColorState(
                state.colorList,
                true
            );
        }
    }

    addColorToList = () => {
        if(this.props.type === "primary"){
            let state = this.props.state.primaryColorState;
            state.colorList.push(newColor);
            this.props.stateUpdaters.updatePrimaryColorState(
                state.colorList,
                state.isPickingColor
            );
        }else{
            let state = this.props.state.secondaryColorState;
            state.colorList.push(newColor);
            this.props.stateUpdaters.updateSecondaryColorState(
                state.colorList,
                state.isPickingColor
            );
        }
    }

    cancelColor = () => {
        if(this.props.type === "primary"){
            let state = this.props.state.primaryColorState;
            this.props.stateUpdaters.updatePrimaryColorState(
                state.colorList,
                false
            );
        }else{
            let state = this.props.state.secondaryColorState;
            this.props.stateUpdaters.updateSecondaryColorState(
                state.colorList,
                false
            );
        }
    }

    handleSelectColor = (color, e) => {
        if(this.props.type === "primary"){
            this.props.state.bluetoothHandler.animation.primaryRed = color.rgb.r;
            this.props.state.bluetoothHandler.animation.primaryGreen = color.rgb.g;
            this.props.state.bluetoothHandler.animation.primaryBlue = color.rgb.b;
            this.props.stateUpdaters.update();
        }else{
            this.props.state.bluetoothHandler.animation.secondaryRed = color.rgb.r;
            this.props.state.bluetoothHandler.animation.secondaryGreen = color.rgb.g;
            this.props.state.bluetoothHandler.animation.secondaryBlue = color.rgb.b;
            this.props.stateUpdaters.update();
        }
    }

    getColorPickerHeaderText = () => {
        if(this.props.type === "primary"){
            return "Select Primary Color";
        }else{
            return "Select Secondary Color";
        }
    }

    getColorList = () => {
        if(this.props.type === "primary"){
            return this.props.state.primaryColorState.colorList;
        }else{
            return this.props.state.secondaryColorState.colorList;
        }
    }

    toggleCustomColorPicker = () => {
        let colorState = (this.props.type === "primary") ? this.props.state.primaryColorState : this.props.state.secondaryColorState
        
        if(colorState.isPickingColor){
            return (
                <div>
                    <SketchPicker presetColors={[]} disableAlpha={true} width="250px" onChangeComplete={this.handleNewColor} color={newColor}/>
                    <button onClick={this.addColorToList}>Add</button>
                    <button onClick={this.cancelColor}>Cancel</button>
                </div>
            )
        }else{
            return <button onClick={this.addColor} >Add Custom Color</button>
        }
    }

    getSelectedColor = () => {
        let selectedColor = null;
        let colorList = null;
        if(this.props.type === "primary"){
            selectedColor = this.getColorString(
                this.props.state.bluetoothHandler.animation.primaryRed,
                this.props.state.bluetoothHandler.animation.primaryGreen,
                this.props.state.bluetoothHandler.animation.primaryBlue
            );
            colorList = this.props.state.primaryColorState.colorList;
            if(colorList.includes(selectedColor)){
                return selectedColor;
            }else{
                this.addColorToList({rgb:{
                    r: this.props.state.bluetoothHandler.animation.primaryRed,
                    g: this.props.state.bluetoothHandler.animation.primaryGreen,
                    b: this.props.state.bluetoothHandler.animation.primaryBlue
                }});
                return selectedColor;
            }
        }else{
            selectedColor = this.getColorString(
                this.props.state.bluetoothHandler.animation.secondaryRed,
                this.props.state.bluetoothHandler.animation.secondaryGreen,
                this.props.state.bluetoothHandler.animation.secondaryBlue
            );
            colorList = this.props.state.secondaryColorState.colorList;
            if(colorList.includes(selectedColor)){
                return selectedColor;
            }else{
                this.addColorToList({rgb:{
                    r: this.props.bluetoothHandler.animation.secondaryRed,
                    g: this.props.bluetoothHandler.animation.secondaryGreen,
                    b: this.props.bluetoothHandler.animation.secondaryBlue
                }});
                return selectedColor;
            }
        }
    }

    getColorString = (r, g, b) => {
        let red = r.toString(16).toUpperCase();
        let green = g.toString(16).toUpperCase();
        let blue = b.toString(16).toUpperCase();
        if(red.length === 1){
            red = "0"+red;
        }
        if(green.length === 1){
            green = "0"+green;
        }
        if(blue.length === 1){
            blue = "0"+blue;
        }
        return "#"+red+green+blue;
    }

    render() {
        return (
            <div className="colorListMainDiv">
                <h3>{this.getColorPickerHeaderText()}</h3>
                <div className="circlePickerDiv">
                    <CirclePicker colors={this.getColorList()} width="295px" onChangeComplete={this.handleSelectColor} color={this.getSelectedColor()}/>
                </div>
                {this.toggleCustomColorPicker()}
            </div>
        )
    }
}

export default ColorList;