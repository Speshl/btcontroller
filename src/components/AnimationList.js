import React, { Component } from 'react'
import './../css/AnimationList.css';

const animations = [
    "Static",
    "Blink",
    "Pulse"/*,
    "Wave (L2R)",
    "Wave (R2L)",
    "Wave (F2B)",
    "Wave (B2F)",
    "Stack (F2B)",
    "Stack (B2F)",
    "RGB Flow"*/
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
        if(isNaN(delay)){
            delay = 0;
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
                    <label className="inRowDisplayLabel">Delay:<input className="inRowDisplayInput" type="number" type="text" name="delay" value={this.getDelayValue()} maxLength="4" onChange={this.updateDelayValue}/></label>
                </div>
            </div>
        )
    }
}

export default AnimationList
