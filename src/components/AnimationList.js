import React, { Component } from 'react'
import './../css/AnimationList.css';

const animations = [
    "Static",
    "Blink",
    "Pulse",
    "Wave",
    "Wave Alternate",
    "Roll",
    "Roll Alternate",
    "Stack",
    "Stack Alternate"
];

export class AnimationList extends Component {

    switchAnimation = (e) => {
        let index = animations.indexOf(e.target.value);
        if(index > -1){
            this.props.stateUpdaters.updateCommandState(index, this.props.state.commandState.delay);
        }
    }

    getSelectedAnimation = () => {
        return animations[this.props.state.commandState.animation];
    }

    getDelayValue = () => {
        return this.props.state.commandState.delay;
    }

    updateDelayValue = (e) => {
        let delay = parseInt(e.target.value);
        if(isNaN(delay) || delay < 10){
            delay = 10;
        }
        this.props.stateUpdaters.updateCommandState(this.props.state.commandState.animation, delay);
    }

    render() {
        return (
            <div className="animationListMainDiv">
                <h3>Select Animation</h3>
                <div>
                    <label className="inRowDisplayLabel">Animation: 
                        <select className="inRowDisplayInput" onChange={this.switchAnimation} value={this.getSelectedAnimation()}>
                            {animations.map(animation => <option key={animation} value={animation}> {animation} </option>)}
                        </select>
                    </label>
                </div>
                <div>
                    <label className="inRowDisplayLabel">Delay:<input className="inRowDisplayInput indentText" type="number" min="10" name="delay" value={this.getDelayValue()} onChange={this.updateDelayValue}/></label>
                </div>
            </div>
        )
    }
}

export default AnimationList
