import React, { Component } from 'react';
import './../css/CommandList.css';

/*emptyCommand = {
    color: null,
    animation: null,
    delay: null
}*/

export class CommandList extends Component {

    buildCommandListRefs = () => {
        var returnArray = [];
        for(let i=0; i<this.props.state.commandList.length; i++){
            returnArray.push(React.createRef());
        }
        return returnArray;
    }

    addCommandToList = () => {
        if(this.props.state.selectedColor != null && this.props.state.selectedAnimation != null && this.props.state.selectedDelay != null)
        {
            if(this.props.state.commandList.length < 10){
                let command = {
                    color: this.props.state.selectedColor,
                    animation: this.props.state.selectedAnimation,
                    delay: this.props.state.selectedDelay
                }
                let newCommandList = this.props.state.commandList.concat([command]);
                this.props.stateUpdaters.updateCommandList(newCommandList);
            }else{
                alert("Limit of 10 commands.");
            }
        }else{
            alert("Please select a color, animation, and delay");
        }
    }
    
    removeCommandFromList = () => {
        let newCommandList = Array.from(this.props.state.commandList);
        let selectedIndex = this.props.state.selectedCommand;//update to get current selection
        newCommandList.splice(selectedIndex,1);
        this.props.stateUpdaters.updateSelectedCommand(null);
        this.props.stateUpdaters.updateCommandList(newCommandList);
    }

    clearCommandList = () => {
        this.props.stateUpdaters.updateSelectedCommand(null);
        this.props.stateUpdaters.updateCommandList([]);
    }

    moveCommandUp = () => {
        if(this.props.state.selectedCommand > 0){
            let modifiableCommandList = Array.from(this.props.state.commandList);
            let commandToMove = modifiableCommandList.splice(this.props.state.selectedCommand,1);
            modifiableCommandList.splice(this.props.state.selectedCommand-1,0,commandToMove[0]);
            this.props.stateUpdaters.updateSelectedCommand(this.props.state.selectedCommand-1);
            this.props.stateUpdaters.updateCommandList(modifiableCommandList);
        }
    }

    moveCommandDown = () => {
        if(this.props.state.selectedCommand + 1 !== this.props.state.commandList.length){
            let modifiableCommandList = Array.from(this.props.state.commandList);
            let commandToMove = modifiableCommandList.splice(this.props.state.selectedCommand,1);
            modifiableCommandList.splice(this.props.state.selectedCommand+1,0,commandToMove[0]);
            this.props.stateUpdaters.updateSelectedCommand(this.props.state.selectedCommand+1);
            this.props.stateUpdaters.updateCommandList(modifiableCommandList);
        }
    }

    getCommandDisplayText = (index) => {
        if(index < this.props.state.commandList.length){
            let selectedCommand = this.props.state.commandList[index];
            return selectedCommand.animation + " " + selectedCommand.color + " for " + selectedCommand.delay + "ms";
        }else{
            return "";
        }
    }

    commandListSelect = (e, selectedIndex) => {
        e.preventDefault();
        this.props.stateUpdaters.updateSelectedCommand(selectedIndex);
    }

    getCommandListDisplay = () => {
        if(this.props.state.commandList.length === 0){
            return <li key="0" className="disabledCommand no-select" onClick={((e) => this.commandListSelect(e, null))}>Add A Command</li>
        }else{
            let commandDisplayText = [];
            for(let i=0; i<this.props.state.commandList.length; i++){
                commandDisplayText.push(this.getCommandDisplayText(i));
            }
            return commandDisplayText.map(
                (commandDisplayText, index) => this.mapCommandDisplayText(commandDisplayText, index)
            );
        }
    }

    mapCommandDisplayText = (commandDisplayText, index) => {
        let className = "";
        if(index === this.props.state.selectedCommand){
            className = "selectedCommand";
        }
        return <li className={className} ref={'command'+index} key={index} onClick={((e) => this.commandListSelect(e, index))}>{commandDisplayText}</li>
    }

    render() {
        return (
            <div className="commandListMainDiv">
                <h3>Step 3: Modify CommandList</h3>
                <ul className="commandListBox">
                    {this.getCommandListDisplay()}
                </ul>
                <div className="buttonDiv">
                    <button onClick={this.addCommandToList}>Add</button>
                    <button onClick={this.removeCommandFromList}>Remove</button>
                    <button onClick={this.clearCommandList}>Clear</button>
                    <button onClick={this.moveCommandUp}>▲</button>
                    <button onClick={this.moveCommandDown}>▼</button>
                </div>
            </div>
        )
    }
}

export default CommandList