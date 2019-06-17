import React, { Component } from 'react';
import ColorList from './ColorList';
import AnimationList from './AnimationList';
import CommandList from './CommandList';
import WriteList from './WriteList';
import './../css/DisplayBuilder.css';

export class DisplayBuilder extends Component {

    getModeToggleButton = () => {
        if(this.props.state.displayMode === "Basic"){
            return <button className="modeButton" onClick={this.props.stateUpdaters.toggleDisplayMode}>Advanced Mode</button>
        }else if(this.props.state.displayMode === "Advanced"){
            return <button className="modeButton" onClick={this.props.stateUpdaters.toggleDisplayMode}>Basic Mode</button>
        }
    }

    determineAdvancedDisplay = () => {
        if(this.props.state.displayMode === "Advanced"){
            return <React.Fragment>
                <AnimationList stateUpdaters={this.props.stateUpdaters} />
                <CommandList state={this.props.state} stateUpdaters={this.props.stateUpdaters} />
                <WriteList state={this.props.state} />
            </React.Fragment>
        }
    }

    render() {
        return (
            <div className="displayBuilderMainDiv">
                <h2>Display Builder</h2>
                {this.getModeToggleButton()}
                <ColorList state={this.props.state} stateUpdaters={this.props.stateUpdaters} />
                {this.determineAdvancedDisplay()}
            </div>
        )
    }
}

export default DisplayBuilder
