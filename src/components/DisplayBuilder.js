import React, { Component } from 'react';
import ColorList from './ColorList';
import AnimationList from './AnimationList';
import ChannelSelector from './ChannelSelector';
import './../css/DisplayBuilder.css';

export class DisplayBuilder extends Component {
    render() {
        return (
            <div className="displayBuilderMainDiv">
                <h2>Display Builder</h2>
                {<ChannelSelector state={this.props.state} stateUpdaters={this.props.stateUpdaters} />}
                <ColorList state={this.props.state} stateUpdaters={this.props.stateUpdaters} type="primary"/>
                <ColorList state={this.props.state} stateUpdaters={this.props.stateUpdaters} type="secondary"/>
                <AnimationList state={this.props.state} stateUpdaters={this.props.stateUpdaters}/>
            </div>
        )
    }
}

export default DisplayBuilder