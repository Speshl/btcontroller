import React, { Component } from 'react';
import ColorList from './ColorList';
import AnimationList from './AnimationList';
import ChannelSelector from './ChannelSelector';
import BlueToothCommands from './../shared/BlueToothCommands';
import './../css/DisplayBuilder.css';

export class DisplayBuilder extends Component {

    writeToDevice = async (e) => {
        try{
            BlueToothCommands.updateCommand(
                this.props.state.deviceState.selectedDevice,
                this.props.state.primaryColorState,
                this.props.state.secondaryColorState,
                this.props.state.commandState
            );
        }catch(err){
            console.log(err);
        }
    }

    render() {
        return (
            <div className="displayBuilderMainDiv">
                <h2>Display Builder</h2>
                <ChannelSelector state={this.props.state} stateUpdaters={this.props.stateUpdaters} />
                <ColorList state={this.props.state} stateUpdaters={this.props.stateUpdaters} type="primary"/>
                <ColorList state={this.props.state} stateUpdaters={this.props.stateUpdaters} type="secondary"/>
                <AnimationList state={this.props.state} stateUpdaters={this.props.stateUpdaters}/>
                <div>
                    <input className="displayBuilderButton" type="button" value="Write To Device" onClick={this.writeToDevice}/>
                </div>
            </div>
        )
    }
}

export default DisplayBuilder