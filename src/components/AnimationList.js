import React, { Component } from 'react';
import './../css/AnimationList.css';

var showStaticDelay = true;
var showBlinkDelay = false;
var showPulseDelay = false;
var showFadeDelay = false;

export class AnimationList extends Component {

    constructor(props){
        super(props);
        this.staticRadioRef = React.createRef();
        this.staticDelayRef = React.createRef();
        this.blinkDelayRef = React.createRef();
        this.pulseDelayRef = React.createRef();
        this.fadeDelayRef = React.createRef();
    }

    radioClicked = (e) => {
        let relatedTextInput = null;
        showStaticDelay = false;
        showBlinkDelay = false;
        showPulseDelay = false;
        showFadeDelay = false;
        switch(e.target.value){
            case "Static":
                showStaticDelay = true;
                relatedTextInput = this.staticDelayRef.current;
                break;
            case "Blink":
                showBlinkDelay = true;
                relatedTextInput = this.blinkDelayRef.current;
                break;
            case "Pulse":
                showPulseDelay = true;
                relatedTextInput = this.pulseDelayRef.current;
                break;
            case "Fade":
                showFadeDelay = true;
                relatedTextInput = this.fadeDelayRef.current;
                break;
            default:
                break;
        }
        this.props.stateUpdaters.updateSelectedAnimation(e.target.value);
        if(relatedTextInput){
            this.props.stateUpdaters.updateSelectedDelay(relatedTextInput.value);
        }
        //this.forceUpdate();
    }

    updateDelay = (e) =>{
        this.props.stateUpdaters.updateSelectedDelay(e.target.value);
    }

    getStaticDelayStyle = () => {
        if(showStaticDelay){
            return {
                display: 'unset'
            }
        }else{
            return {
                display: 'none'
            }
        }
    }

    getBlinkDelayStyle = () => {
        if(showBlinkDelay){
            return {
                display: 'unset'
            }
        }else{
            return {
                display: 'none'
            }
        }
    }

    getPulseDelayStyle = () => {
        if(showPulseDelay){
            return {
                display: 'unset'
            }
        }else{
            return {
                display: 'none'
            }
        }
    }

    getFadeDelayStyle = () => {
        if(showFadeDelay){
            return {
                display: 'unset'
            }
        }else{
            return {
                display: 'none'
            }
        }
    }

    applyDefaultSelection = () => {
        this.staticRadioRef.current.click();
    }

    render() {
        return (
            <div className="animationListMainDiv">
                <h3>Step 2: Choose Animation</h3>
                <div>
                    <label className="radioLabel"><input type="radio" name="animation" value="Static" onClick={this.radioClicked} ref={this.staticRadioRef}/>Static</label>
                    <label className="textLabel" style={this.getStaticDelayStyle()}>Delay (ms):<input type="text" name="staticDelay" ref={this.staticDelayRef} defaultValue="100" maxLength="5" onKeyPress={this.updateDelay}/></label>
                </div>
                <div>
                    <label className="radioLabel"><input type="radio" name="animation" value="Blink" onClick={this.radioClicked}/>Blink</label>
                    <label className="textLabel" style={this.getBlinkDelayStyle()}>Delay (ms):<input type="text" name="blinkDelay" ref={this.blinkDelayRef} defaultValue="100" maxLength="5"  onKeyPress={this.updateDelay}/></label>
                </div>
                <div>
                    <label className="radioLabel"><input type="radio" name="animation" value="Pulse" onClick={this.radioClicked}/>Pulse</label>
                    <label className="textLabel" style={this.getPulseDelayStyle()}>Delay (ms):<input type="text" name="pulseDelay" ref={this.pulseDelayRef} defaultValue="100" maxLength="5" onKeyPress={this.updateDelay}/></label>
                </div>
                <div>
                    <label className="radioLabel"><input type="radio" name="animation" value="Fade" onClick={this.radioClicked}/>Fade</label>
                    <label className="textLabel" style={this.getFadeDelayStyle()}>Delay (ms):<input type="text" name="fadeDelay" ref={this.fadeDelayRef} defaultValue="100" maxLength="5" onKeyPress={this.updateDelay}/></label>
                </div>
            </div>
        )
    }

    componentDidMount = () => {
        this.applyDefaultSelection();
    }
}

export default AnimationList
