import React, { Component } from 'react'
import { SketchPicker } from 'react-color';
import { CirclePicker } from 'react-color';
import './../css/ColorList.css';

var newColor = null;

class ColorList extends Component {

    handleNewColor = (color, e) => {
        newColor = color.hex.toUpperCase();
    }

    addColor = (e) => {
        if(this.props.type === "primary"){
            let state = this.props.state.primaryColorState;
            this.props.stateUpdaters.updatePrimaryColorState(
                state.colorList,
                true,
                state.red,
                state.green,
                state.blue
            );
        }else{
            let state = this.props.state.secondaryColorState;
            this.props.stateUpdaters.updateSecondaryColorState(
                state.colorList,
                true,
                state.red,
                state.green,
                state.blue
            );
        }
    }

    addColorToList = () => {
        if(this.props.type === "primary"){
            let state = this.props.state.primaryColorState;
            state.colorList.push(newColor);
            this.props.stateUpdaters.updatePrimaryColorState(
                state.colorList,
                state.isPickingColor,
                state.red,
                state.green,
                state.blue
            );
        }else{
            let state = this.props.state.secondaryColorState;
            state.colorList.push(newColor);
            this.props.stateUpdaters.updateSecondaryColorState(
                state.colorList,
                state.isPickingColor,
                state.red,
                state.green,
                state.blue
            );
        }
    }

    cancelColor = () => {
        if(this.props.type === "primary"){
            let state = this.props.state.primaryColorState;
            this.props.stateUpdaters.updatePrimaryColorState(
                state.colorList,
                false,
                state.red,
                state.green,
                state.blue
            );
        }else{
            let state = this.props.state.secondaryColorState;
            this.props.stateUpdaters.updateSecondaryColorState(
                state.colorList,
                false,
                state.red,
                state.green,
                state.blue
            );
        }
    }

    handleSelectColor = (color, e) => {
        if(this.props.type === "primary"){
            let state = this.props.state.primaryColorState;
            this.props.stateUpdaters.updatePrimaryColorState(
                state.colorList,
                state.isPickingColor,
                color.rgb.r,
                color.rgb.g,
                color.rgb.b
            );
        }else{
            let state = this.props.state.secondaryColorState;
            this.props.stateUpdaters.updateSecondaryColorState(
                state.colorList,
                state.isPickingColor,
                color.rgb.r,
                color.rgb.g,
                color.rgb.b
            );
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
                    <SketchPicker presetColors={[]} disableAlpha={true} width="250px" onChangeComplete={this.handleNewColor}/>
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
                this.props.state.primaryColorState.red,
                this.props.state.primaryColorState.green,
                this.props.state.primaryColorState.blue
            );
            colorList = this.props.state.primaryColorState.colorList;
            if(colorList.includes(selectedColor)){
                return selectedColor;
            }else{
                this.addColorToList({rgb:{
                    r: this.props.state.primaryColorState.red,
                    g: this.props.state.primaryColorState.green,
                    b: this.props.state.primaryColorState.blue
                }});
            }
        }else{
            selectedColor = this.getColorString(
                this.props.state.secondaryColorState.red,
                this.props.state.secondaryColorState.green,
                this.props.state.secondaryColorState.blue
            );
            colorList = this.props.state.secondaryColorState.colorList;
            if(colorList.includes(selectedColor)){
                return selectedColor;
            }else{
                this.addColorToList({rgb:{
                    r: this.props.state.secondaryColorState.red,
                    g: this.props.state.secondaryColorState.green,
                    b: this.props.state.secondaryColorState.blue
                }});
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