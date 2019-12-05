//Was used initially when I had a nano with the HM-10 module.  Was working to send bluetooth string messages. Not using any longer because nano 33 IOT has support for multiple services and characteristics
var serviceUUIDs = require('./uuids');

export default class BlueToothCommands {
    static encoder = new TextEncoder();
    static decoder = new TextDecoder('utf-8');

    static connect = async (disconnectListener) => {
        let uuidArray = (serviceUUIDs.channelUUIDs.concat(serviceUUIDs.commandUUIDs));
        uuidArray.push(serviceUUIDs.generalUUIDs[0]);
        let options = {
            acceptAllDevices: true,
            optionalServices: uuidArray,
            /*filters:[
                {services: ['19b10010-e8f2-537e-4f6c-d104768a1214']}
            ]*/
        };
        try{
            let device = await navigator.bluetooth.requestDevice(options);
            device.addEventListener('gattserverdisconnected', disconnectListener);
            let server = await device.gatt.connect();
            return server;
        }catch(e){
            throw new Error("Bluetooth Connection: "+e);
        }
    }

    static disconnect = (server) => {
        if(this.isConnected(server)){
            server.disconnect();
        }
    }

    static isConnected = (server) => {
        let connected = false;
        if(server && server.connected){
          connected = true;
        }
        return connected;
    }

    static load = async (server) => {
        try{
            let dataArray = await Promise.all([
                this.loadGeneral(server),
                this.loadChannels(server),
                this.loadCommands(server)
            ]);
            return dataArray;
        }catch(error){
            console.log("Error loading all services: "+error);
            return null;
        }
    }

    static loadGeneral = async (server) => {
        try{
            let service = await server.getPrimaryService(serviceUUIDs.generalUUIDs[0]);
            let lightCharacteristic = await service.getCharacteristic(serviceUUIDs.generalUUIDs[1]);
            //let saveCharacteristic = await service.getCharacteristic(serviceUUIDs.generalUUIDs[2]);
            let lightStatus = await lightCharacteristic.readValue();
            return {lightStatus: (lightStatus.getUint8(0) === 1) ? true : false};
        }catch(error){
            console.log("Error loading general data: "+error);
            return null;
        }
    }

    static loadChannels = async (server) => {
        try{
            let channelServices = await Promise.all([
                server.getPrimaryService(serviceUUIDs.channelUUIDs[0]),
                server.getPrimaryService(serviceUUIDs.channelUUIDs[1]),
                server.getPrimaryService(serviceUUIDs.channelUUIDs[2]),
                server.getPrimaryService(serviceUUIDs.channelUUIDs[3]),
                server.getPrimaryService(serviceUUIDs.channelUUIDs[4]),
                server.getPrimaryService(serviceUUIDs.channelUUIDs[5]),
                server.getPrimaryService(serviceUUIDs.channelUUIDs[6]),
                server.getPrimaryService(serviceUUIDs.channelUUIDs[7])
            ]);
    
            let channelData = await Promise.all([
                this.loadChannelsData(channelServices[0], 0),
                this.loadChannelsData(channelServices[1], 1),
                this.loadChannelsData(channelServices[2], 2),
                this.loadChannelsData(channelServices[3], 3),
                this.loadChannelsData(channelServices[4], 4),
                this.loadChannelsData(channelServices[5], 5),
                this.loadChannelsData(channelServices[6], 6),
                this.loadChannelsData(channelServices[7], 7)
            ]);
            return channelData;
        }catch(error){
            console.log("Error loading channels: "+error);
            return null;
        }
    }

    static loadChannelsData = async (service, channelIndex) => {
        try{
            let [numLEDscharacteristic, stripTypeCharacteristic, positionCharacteristic] = await Promise.all([
                service.getCharacteristic(serviceUUIDs.channelNumLEDsUUIDs[channelIndex]),
                service.getCharacteristic(serviceUUIDs.channelStripTypeUUIDs[channelIndex]),
                service.getCharacteristic(serviceUUIDs.channelPositionUUIDs[channelIndex])
            ]);
    
            let [numLEDsData, stripTypeData, positionData] = await Promise.all([
                numLEDscharacteristic.readValue(),
                stripTypeCharacteristic.readValue(),
                positionCharacteristic.readValue()
            ]);

            let returnObject = {
                numLEDs: this.getIntFromUint8Array(numLEDsData),
                stripData: this.getTypeAndOrderFromUint8Array(stripTypeData),
                position: this.getPositionFromUint8Array(positionData)
            }
            return returnObject;
        }catch(error){
            console.log("Error loading channelData: "+error.stack);
            return null;
        }
    }

    static loadCommands = async (server) => {
        try{
            let commandServices = await Promise.all([
                server.getPrimaryService(serviceUUIDs.commandUUIDs[0])
            ]);
    
            let commandData = await Promise.all([
                this.loadCommandsData(commandServices[0], 0)
            ]);
            return commandData;
        }catch(error){
            console.log("Error loading commands: "+error);
            return null;
        }
    }

    static loadCommandsData = async (service, commandIndex) => {
        try{
            let [
                colorChar,
                delayChar,
                animationChar,
                usedChannelsChar
            ] = await Promise.all([
                service.getCharacteristic(serviceUUIDs.commandColorUUIDs[commandIndex]),
                service.getCharacteristic(serviceUUIDs.commandDelayUUIDs[commandIndex]),
                service.getCharacteristic(serviceUUIDs.commandAnimationUUIDs[commandIndex]),
                service.getCharacteristic(serviceUUIDs.commandChannelToggleUUIDs[commandIndex])
            ]);

            let [
                colorData,
                delayData,
                animationData,
                usedChannelsData
            ] = await Promise.all([
                colorChar.readValue(),
                delayChar.readValue(),
                animationChar.readValue(),
                usedChannelsChar.readValue()
            ]);
            let color = this.getColorFromUint8Array(colorData);
            let returnObject = {
                red: color.red,
                green: color.green,
                blue: color.blue,
                delay: this.getIntFromUint8Array(delayData),
                animation: animationData.getUint8(0),
                usedChannels: this.getUsedChannelsFromInt(usedChannelsData)
            }
            return returnObject;
        }catch(error){
            console.log("Error loading commandData: "+error);
            return null;
        }
    }

    static save = async (server) => {
        try{
            let service = await server.getPrimaryService(serviceUUIDs.generalUUIDs[0]);
            let characteristic = await service.getCharacteristic(serviceUUIDs.generalUUIDs[2]);
            let value = new Uint8Array([1]);//false is [0]
            characteristic.writeValue(value);
            return true;
        }catch(error){
            console.log("Error sending save command: "+error);
            return false;
        }
    }

    static toggleLights = async (server, status) => {
        try{
            let value = (status) ? new Uint8Array([0]) : new Uint8Array([1])
            let service = await server.getPrimaryService(serviceUUIDs.generalUUIDs[0]);
            let characteristic = await service.getCharacteristic(serviceUUIDs.generalUUIDs[1]);
            characteristic.writeValue(value);
            return true;
        }catch(error){
            console.log("Error sending toggleLights command: "+error);
            return false;
        }
    }

    static updateChannel = async (server, channelIndex, stripType, numLEDs, position, order) =>{
        try{
            let service = await server.getPrimaryService(serviceUUIDs.channelUUIDs[channelIndex]);
            let [numLEDscharacteristic, stripTypeCharacteristic, positionCharacteristic] = await Promise.all([
                service.getCharacteristic(serviceUUIDs.channelNumLEDsUUIDs[channelIndex]),
                service.getCharacteristic(serviceUUIDs.channelStripTypeUUIDs[channelIndex]),
                service.getCharacteristic(serviceUUIDs.channelPositionUUIDs[channelIndex])
            ]);

            await Promise.all([
                numLEDscharacteristic.writeValue(this.getUint8ArrayFromInt(numLEDs)),
                stripTypeCharacteristic.writeValue(this.getUint8ArrayFromTypeAndOrder(stripType, order)),
                positionCharacteristic.writeValue(this.getUint8ArrayFromPosition(position))
            ]);
            return true;
        }catch(error){
            console.log("Error sending updateChannel command: "+error);
            return false;
        }
    }

    static updateCommand = async (server, primaryColor, secondaryColor, command, usedChannels) => {
        try{
            let service = await server.getPrimaryService(serviceUUIDs.commandUUIDs[0]);

            let [
                colorChar,
                delayChar,
                animationChar,
                usesChannelChar
            ] = await Promise.all([
                service.getCharacteristic(serviceUUIDs.commandColorUUIDs[0]),
                service.getCharacteristic(serviceUUIDs.commandDelayUUIDs[0]),
                service.getCharacteristic(serviceUUIDs.commandAnimationUUIDs[0]),
                service.getCharacteristic(serviceUUIDs.commandChannelToggleUUIDs[0])
            ]);

            await Promise.all([
                colorChar.writeValue(this.getUint8ArrayFromColor(primaryColor.red, primaryColor.green, primaryColor.blue)),
                delayChar.writeValue(this.getUint8ArrayFromInt(command.delay)),
                animationChar.writeValue(this.getUint8ArrayFromInt(command.animation)),
                usesChannelChar.writeValue(this.getIntFromUsedChannels(usedChannels))
            ]);

            return true;
        }catch(error){
            console.log("Error sending updateCommand ... command: "+error);
            return false;
        }
    }

    static getUint8ArrayFromInt = (value) => {
        let builtArray = [];
        let valueLeft = value;
        for(let i=0; i<8; i++){
            if(valueLeft > 255){
                builtArray.push(255);
                valueLeft -= 255;
            }else{
                builtArray.push(valueLeft);
                valueLeft = 0;
            }
        }
        return new Uint8Array(builtArray);
    }

    static getIntFromUint8Array = (value) => {
        let returnValue = 0;
        try{
            for(let i=0; i<value.byteLength; i++){
                returnValue += value.getUint8(i);
            }
            return returnValue;
        }catch(error){
            return 0;
        }
    }

    static getUint8ArrayFromColor = (red, green, blue) => {
        return new Uint8Array([red, green, blue, 0,0,0,0,0]);
    }

    static getColorFromUint8Array = (value) => {
        return{red:value.getUint8(0), green:value.getUint8(1), blue:value.getUint8(2)};
    }

    static getUint8ArrayFromPosition = (value) => {
        return new Uint8Array([parseInt(value.charAt(0)), parseInt(value.charAt(2)), 0,0,0,0,0,0]);
    }

    static getPositionFromUint8Array = (value) => {
        let returnString = "";
        returnString += value.getUint8(0) + ":" +value.getUint8(1);
        return returnString;
    }

    static getTypeAndOrderFromUint8Array = (value) => {
        return {type: value.getUint8(0), order: value.getUint8(1)};
    }

    static getUint8ArrayFromTypeAndOrder = (type, order) => {
        return new Uint8Array([type, order, 0,0,0,0,0,0]);
    }

    static getUsedChannelFromInt2 = (index, value) => {
        return (value >> index === 1) ? true : false
    }

    static getUsedChannelsFromInt = (value) => {
        let byte = value.getUint8(0);
        let returnArray = [false,false,false,false,false,false,false,false];
        let used = [1,2,4,8,16,32,64,128];
        for(let i=7; i>=0; i--){
            if(byte >= used[i]){
                byte -= used[i];
                returnArray[i] = true;
            }
        }
        return returnArray;
    }

    static getIntFromUsedChannels = (usedChannels) => {
        let buffer = new ArrayBuffer(4);
        let used = [1,2,4,8,16,32,64,128];
        for(let i=0; i<usedChannels; i++){
            if(usedChannels[i].used === true){
                buffer[0] += used[i];
            }
        }
        return buffer;
    }
}