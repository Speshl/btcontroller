import React, { Component } from 'react'
import BlueToothCommands from './../shared/BlueToothCommands';
import './../css/WriteList.css';

export class WriteList extends Component {

    writeToDevice = async () => {
        try{
            return await BlueToothCommands.writeToDevice(this.props.state.selectedDevice, this.props.state.commandList);
        }catch(e){
            alert(e);
        }
    }

    generateButton = () => {
        if(!this.props.state.selectedDevice || !this.props.state.selectedDevice.gatt){
            return <button onClick={this.writeToDevice} className="buttonDisabled" disabled>CONNECT TO DEVICE FIRST!</button>
        }else if(this.props.state.commandList.length === 0){
            return <button onClick={this.writeToDevice} className="buttonDisabled" disabled>ADD TO COMMAND LIST FIRST!</button>
        }else{
            return <button onClick={this.writeToDevice}>WRITE COMMANDLIST TO DEVICE!</button>
        }
    }

    render() {
        return (
            <div className="writeListMainDiv">
                <h3>Step 4: Write To Controller</h3>
                {this.generateButton()}
            </div>
        )
    }
}

export default WriteList
