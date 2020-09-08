import React, { Component } from 'react'
import './../css/ChannelSelector.css';
import Channel from './../classes/Channel';

export class ChannelSelector extends Component {
    updateChannel = async (e) => {
        if(this.props.state.editedChannel.id !== null){
            let id = this.props.state.editedChannel.id;
            this.props.state.bluetoothHandler.channels[id] = this.props.state.editedChannel;
            this.props.stateUpdaters.update();
        }
    }

    getIntFromPosition = (position) => {
        let intPosition = 0;
        let splitPosition = position.split(":");
        intPosition = parseInt(splitPosition[0]) * 4
        intPosition += parseInt(splitPosition[1]);
        return intPosition;
    }

    channelClicked = (e) => {
        let id = e.target.value;
        this.props.state.bluetoothHandler.channels[id].selected = true;
        this.props.stateUpdaters.update();
    }

    getTogglePositionStyle = (value) => {
        if(this.props.state.editedChannel.id !== null){
            let location = this.props.state.editedChannel.location;
            if(location !== null && location === value){
                return "channelPositionSelectedUsed";
            }else{
                return "channelPosition";
            }
        }
    }

    switchEditingChannel = (e) => {
        this.props.state.editedChannel = this.props.state.bluetoothHandler.channels[e.target.value];
        this.props.stateUpdaters.update();
    }

    updateChannelPosition = (e) => {
        let value = (e.target.value === "") ? 0 : parseInt(e.target.value)
        if(this.props.state.editedChannel.id !== null){
            this.props.state.editedChannel.location = value;
            this.props.stateUpdaters.update();
        }
    }

    updateStripType = (e) => {
        let value = (e.target.value === "") ? 0 : parseInt(e.target.value)
        if(this.props.state.editedChannel.type !== null){
            this.props.state.editedChannel.type = value;
            this.props.stateUpdaters.update();
        }
    }

    updateEnabled = (e) => {
        if(this.props.state.editedChannel.id !== null){
            this.props.state.editedChannel.enabled = e.target.checked;
            this.props.stateUpdaters.update();
        }
    }

    updateFlipped = (e) => {
        if(this.props.state.editedChannel.id !== null){
            this.props.state.editedChannel.directionFlipped = e.target.checked;
            this.props.stateUpdaters.update();
        }
    }

    updateLeftTurn = (e) => {
        if(this.props.state.editedChannel.id !== null){
            this.props.state.editedChannel.leftTurn = e.target.checked;
            this.props.stateUpdaters.update();
        }
    }

    updateRightTurn = (e) => {
        if(this.props.state.editedChannel.id !== null){
            this.props.state.editedChannel.rightTurn = e.target.checked;
            this.props.stateUpdaters.update();
        }
    }

    updateBrakeState = (e) => {
        if(this.props.state.editedChannel.id !== null){
            this.props.state.editedChannel.brake = e.target.checked;
            this.props.stateUpdaters.update();
        }
    }

    updateReverseState = (e) => {
        if(this.props.state.editedChannel.id !== null){
            this.props.state.editedChannel.reverse = e.target.checked;
            this.props.stateUpdaters.update();
        }
    }

    updateInterior = (e) => {
        if(this.props.state.editedChannel.id !== null){
            this.props.state.editedChannel.interior = e.target.checked;
            this.props.stateUpdaters.update();
        }
    }

    updateCentered = (e) => {
        if(this.props.state.editedChannel.id !== null){
            this.props.state.editedChannel.centered = e.target.checked;
            this.props.stateUpdaters.update();
        }
    }

    updateNumLEDs = (e) => {
        let value = (e.target.value === "") ? 0 : parseInt(e.target.value)
        if(this.props.state.editedChannel.id !== null){
            this.props.state.editedChannel.numLEDs = value;
            this.props.stateUpdaters.update();
        }
    }

    updateHeight = (e) => {
        let value = (e.target.value === "") ? 0 : parseInt(e.target.value)
        if(this.props.state.editedChannel.id !== null){
            this.props.state.editedChannel.height = value;
            this.props.stateUpdaters.update();
        }
    }

    updateWidth = (e) => {
        let value = (e.target.value === "") ? 0 : parseInt(e.target.value)
        if(this.props.state.editedChannel.id !== null){
            this.props.state.editedChannel.width = value;
            this.props.stateUpdaters.update();
        }
    }

    updateColorOrder = (e) => {
        let value = (e.target.value === "") ? 0 : parseInt(e.target.value)
        if(this.props.state.editedChannel.id !== null){
            this.props.state.editedChannel.order = value;
            this.props.stateUpdaters.update();
        }
    }

    addEditChannels = () => {
        if(this.props.state.editedChannel.id === null){
            this.props.state.editedChannel = this.props.state.bluetoothHandler.channels[0];
        }else{
            this.props.state.editedChannel = new Channel(null);
        }
        this.props.stateUpdaters.update();
    }

    getNumLEDsValue = () => {
        if(this.props.state.editedChannel.id !== null){
            
            let numLEDs = this.props.state.editedChannel.numLEDs;
            if(numLEDs !== null){
                return numLEDs
            }else{
                return 0;
            }
        }else{
            return 0;
        }
    }

    getHeight = () => {
        if(this.props.state.editedChannel.id !== null){
            let height = this.props.state.editedChannel.height;
            if(height !== null){
                return height
            }else{
                return 0;
            }
        }else{
            return 0;
        }
    }

    getWidth = () => {
        if(this.props.state.editedChannel.id !== null){    
            let width = this.props.state.editedChannel.width;
            if(width !== null){
                return width
            }else{
                return 0;
            }
        }else{
            return 0;
        }
    }

    getSelectedChannel = () => {
        if(this.props.state.editedChannel.id !== null){
            return this.props.state.editedChannel.id;
        }else{
            return 0;
        }
    }
    
    getSelectedColorOrder = () => {
        if(this.props.state.editedChannel.id !== null){
            let order = this.props.state.editedChannel.order;
            if(order !== null){
                return order
            }else{
                return 0;
            }
        }else{
            return 0;
        }
    }

    getSelectedStripType = () => {
        if(this.props.state.editedChannel.id !== null){
            let type = this.props.state.editedChannel.type;
            if(type !== null){
                return type
            }else{
                return 0;
            }
        }else{
            return 0;
        }
    }

    getInteriorCheckedState = () => {
        if(this.props.state.editedChannel.id !== null){
            let value = this.props.state.editedChannel.interior;
            if(value !== null){
                return value
            }else{
                return false;
            }
        }else{
            return false;
        }
    }

    getCenteredCheckedState = () => {
        if(this.props.state.editedChannel.id !== null){
            let value = this.props.state.editedChannel.centered;
            if(value !== null){
                return value
            }else{
                return false;
            }
        }else{
            return false;
        }
    }

    getEnabledCheckedState = () => {
        if(this.props.state.editedChannel.id !== null){
            let value = this.props.state.editedChannel.enabled;
            if(value !== null){
                return value
            }else{
                return false;
            }
        }else{
            return false;
        }
    }

    getFlippedCheckedState = () => {
        if(this.props.state.editedChannel.id !== null){
            let value = this.props.state.editedChannel.directionFlipped;
            if(value !== null){
                return value
            }else{
                return false;
            }
        }else{
            return false;
        }
    }

    getRightTurnCheckedState = () => {
        if(this.props.state.editedChannel.id !== null){
            let value = this.props.state.editedChannel.rightTurn;
            if(value !== null){
                return value
            }else{
                return false;
            }
        }else{
            return false;
        }
    }

    getLeftTurnCheckedState = () => {
        if(this.props.state.editedChannel.id !== null){
            let value = this.props.state.editedChannel.leftTurn;
            if(value !== null){
                return value
            }else{
                return false;
            }
        }else{
            return false;
        }
    }

    getBrakeCheckedState = () => {
        if(this.props.state.editedChannel.id !== null){
            let value = this.props.state.editedChannel.brake;
            if(value !== null){
                return value
            }else{
                return false;
            }
        }else{
            return false;
        }
    }

    getReverseCheckedState = () => {
        if(this.props.state.editedChannel.id !== null){
            let value = this.props.state.editedChannel.reverse;
            if(value !== null){
                return value
            }else{
                return false;
            }
        }else{
            return false;
        }
    }

    showAddEditDescription = () => {
        if(this.props.state.editedChannel.id !== null){
            return <React.Fragment>
                <div className="channelAddEditDiv">
                    <div className='row'>
                        <label className='column leftColumn'>Channel:</label>
                        <div className="column">
                            <select className="channelInput rightColumn" onChange={this.switchEditingChannel} value={this.getSelectedChannel()}>
                                <option value={0}>0</option>
                                <option value={1}>1</option>
                                <option value={2}>2</option>
                                <option value={3}>3</option>
                                <option value={4}>4</option>
                                <option value={5}>5</option>
                                <option value={6}>6</option>
                                <option value={7}>7</option>
                                <option value={8}>8</option>
                                <option value={9}>9</option>
                                <option value={10}>10</option>
                                <option value={11}>11</option>
                                <option value={12}>12</option>
                                <option value={13}>13</option>
                                <option value={14}>14</option>
                                <option value={15}>15</option>
                            </select>
                        </div>
                    </div>
                    <div className='row'>
                        <label className='column leftColumn'>LED Count:</label>
                        <div className="column">
                            <input className="channelTextInput rightColumn" type="text" name="numLEDs" value={this.getNumLEDsValue()} maxLength="3" onChange={this.updateNumLEDs}/> 
                        </div>
                    </div>
                    <div className='row'>
                        <label className='column leftColumn'>Height:</label>
                        <div className="column">
                            <input className="channelTextInput rightColumn" type="text" name="height" value={this.getHeight()} maxLength="3" onChange={this.updateHeight}/> 
                        </div>
                    </div>
                    <div className='row'>
                        <label className='column leftColumn'>Width:</label>
                        <div className="column">
                            <input className="channelTextInput rightColumn" type="text" name="width" value={this.getWidth()} maxLength="3" onChange={this.updateWidth}/> 
                        </div>
                    </div>
                    <div className='row'>
                        <label className='column leftColumn'>Color Order:</label>
                        <div className="column">
                            <select className="channelInput rightColumn" value={this.getSelectedColorOrder()} onChange={this.updateColorOrder}>
                                <option value={0}>RGB</option>
                                <option value={1}>RBG</option>
                                <option value={2}>BRG</option>
                                <option value={3}>BGR</option>
                                <option value={4}>GBR</option>
                                <option value={5}>GRB</option>
                            </select>
                        </div>                        
                    </div>
                    <div className='row'>
                        <label className='column leftColumn'>Strip Type:</label>
                        <div className="column">
                            <select className="channelInput rightColumn" value={this.getSelectedStripType()} onChange={this.updateStripType}>
                                <option value={0}>NONE</option>
                                <option value={1}>NEO KHZ800</option>
                                <option value={2}>NEO KHZ400</option>
                            </select>
                        </div>
                    </div>
                    <div className='row'>
                        <div className='column inlineColumnLeft'>
                            <input name="isEnabled" type="checkbox" checked={this.getEnabledCheckedState()} onChange={this.updateEnabled}/>
                            <label>Enabled?</label>
                        </div>
                        <div className="column inlineColumnRight">
                            <input name="isFlipped" type="checkbox" checked={this.getFlippedCheckedState()} onChange={this.updateFlipped}/>
                            <label>Flipped?</label>
                        </div>
                    </div>
                    <div className='row'>
                        <div className='column inlineColumnLeft'>
                            <input name="isInterior" type="checkbox" checked={this.getInteriorCheckedState()} onChange={this.updateInterior}/>
                            <label>In Interior?</label>
                        </div>
                        <div className="column inlineColumnRight">
                            <input name="isCentered" type="checkbox" checked={this.getCenteredCheckedState()} onChange={this.updateCentered}/>
                            <label>Is Centered?</label>
                        </div>
                    </div>
                    <div className='row'>
                        <div className='column inlineColumnLeft'>
                            <input name="rightTurn" type="checkbox" checked={this.getRightTurnCheckedState()} onChange={this.updateRightTurn}/>
                            <label>Right Signal?</label>
                        </div>
                        <div className="column inlineColumnRight">
                            <input name="leftTurn" type="checkbox" checked={this.getLeftTurnCheckedState()} onChange={this.updateLeftTurn}/>
                            <label>Left Signal?</label>
                        </div>
                    </div>
                    <div className='row'>
                        <div className='column inlineColumnLeft'>
                            <input name="brake" type="checkbox" checked={this.getBrakeCheckedState()} onChange={this.updateBrakeState}/>
                            <label>Brake Signal?</label>
                        </div>
                        <div className="column inlineColumnRight">
                            <input name="reverse" type="checkbox" checked={this.getReverseCheckedState()} onChange={this.updateReverseState}/>
                            <label>Reverse Light?</label>
                        </div>
                    </div>
                    <div className="channelPositionDiv" >
                        <label>Strip Position:
                            <div>
                                <input className={this.getTogglePositionStyle("0:0")} type="button" value="0:0" onClick={this.updateChannelPosition}/>
                                <input className={this.getTogglePositionStyle("0:1")} type="button" value="0:1" onClick={this.updateChannelPosition}/>
                                <input className={this.getTogglePositionStyle("0:2")} type="button" value="0:2" onClick={this.updateChannelPosition}/>
                            </div>  
                            <div>
                                <input className={this.getTogglePositionStyle("1:0")} type="button" value="1:0" onClick={this.updateChannelPosition}/>
                                <input className={this.getTogglePositionStyle("1:1")} type="button" value="1:1" onClick={this.updateChannelPosition}/>
                                <input className={this.getTogglePositionStyle("1:2")} type="button" value="1:2" onClick={this.updateChannelPosition}/>
                            </div>  
                            <div>
                                <input className={this.getTogglePositionStyle("2:0")} type="button" value="2:0" onClick={this.updateChannelPosition}/>
                                <input className={this.getTogglePositionStyle("2:1")} type="button" value="2:1" onClick={this.updateChannelPosition}/>
                                <input className={this.getTogglePositionStyle("2:2")} type="button" value="2:2" onClick={this.updateChannelPosition}/>
                            </div>  
                            <div>
                                <input className={this.getTogglePositionStyle("3:0")} type="button" value="3:0" onClick={this.updateChannelPosition}/>
                                <input className={this.getTogglePositionStyle("3:1")} type="button" value="3:1" onClick={this.updateChannelPosition}/>
                                <input className={this.getTogglePositionStyle("3:2")} type="button" value="3:2" onClick={this.updateChannelPosition}/>
                            </div>  

                        </label>
                    </div>
                    <div>
                        <input className="channelButton channelInput" type="button" value="Update Channel" onClick={this.updateChannel}/>
                    </div>
                </div>
            </React.Fragment>
        }
    }

    render() {
        return (
            <div className="channelSelectorMainDiv">
                <h3>Modify Channels</h3>
                <div className="channelsDiv">
                    <input className="channelButton channelInput" type="button" name="addEditChannels" value="Toggle Channel Editor" onClick={this.addEditChannels}/>
                    {this.showAddEditDescription()}
                </div>
            </div>
        )
    }
}

export default ChannelSelector
