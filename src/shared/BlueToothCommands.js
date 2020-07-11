var serviceUUIDs = require('./uuids');

export default class BlueToothCommands {
    static encoder = new TextEncoder();
    static decoder = new TextDecoder('utf-8');

    static connect = async (disconnectListener) => {
        let uuidArray = serviceUUIDs.channelUUIDs;
        uuidArray.push(serviceUUIDs.saveUUID);
        uuidArray.push(serviceUUIDs.onUUID);
        uuidArray.push(serviceUUIDs.commandUUID);
        uuidArray.push(serviceUUIDs.serviceUUID);
        let options = {
            acceptAllDevices: true,
            optionalServices: uuidArray,
            /*filters:[
                {services: [serviceUUIDs.serviceUUID]}
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

    static bulkLoad = async (server) => {
        if(!server.connected){
            server = await server.connect();
        }
        let service = await server.getPrimaryService(serviceUUIDs.serviceUUID);
        let char = await service.getCharacteristic(serviceUUIDs.bulkReadUUID);
        let data = await char.readValue();

        let lightsOn = (data.getUint8(0) === 1) ? true : false;
        let interiorLightsOn = (data.getUint8(1) === 1) ? true : false;
        let command = this.readCommandData(data.buffer.slice(2,12))

        let channelsData = [];
        for(let i=0; i< 8; i++){
            let startIndex = 12+(14*i);
            let endIndex = startIndex + 14;
            channelsData.push(this.readChannelData(data.buffer.slice(startIndex, endIndex)));
        }
        
        return {
            lightsOn: lightsOn,
            interiorLightsOn: interiorLightsOn,
            command: command,
            channels: channelsData
        }
    }

    static load = async (server) => {
        if(!server.connected){
            server = await server.connect();
        }
        let service = await server.getPrimaryService(serviceUUIDs.serviceUUID);
        let data = await Promise.all([
            this.loadLightsOnData(service),
            this.loadCommandData(service),
            this.loadChannelData(service, 0),
            this.loadChannelData(service, 1),
            this.loadChannelData(service, 2),
            this.loadChannelData(service, 3),
            this.loadChannelData(service, 4),
            this.loadChannelData(service, 5),
            this.loadChannelData(service, 6),
            this.loadChannelData(service, 7)
        ]);
        return {
            lightsOn: data[0],
            command: data[1],
            channels: data.slice(2)
        }
    }

    static loadLightsOnData = async (service) => {
        try{
            let char = await service.getCharacteristic(serviceUUIDs.onUUID);
            let value = await char.readValue();
            return (value.getUint8(0) === 1) ? true : false;
        }catch(error){
            console.log(error);
            return true;
        }
    }

    static readCommandData = (value) => {
        let uInt8Viewer = new Uint8Array(value);
        let uInt16Viewer = new Uint16Array(value);
        return {
            primaryRed: uInt8Viewer[0],
            primaryGreen: uInt8Viewer[1],
            primaryBlue: uInt8Viewer[2],
            secondaryRed: uInt8Viewer[3],
            secondaryGreen: uInt8Viewer[4],
            secondaryBlue: uInt8Viewer[5],
            animation: uInt8Viewer[6],
            stepDelay: uInt16Viewer[4]
        }
    }

    static loadCommandData = async (service) => {
        try{
            let char = await service.getCharacteristic(serviceUUIDs.commandUUID);
            let value = await char.readValue();
            return this.readCommandData(value);
        }catch(error){
            console.log("Error reading commandData: "+error);
            return{
                primaryRed: 0,
                primaryGreen: 0,
                primaryBlue: 0,
                secondaryRed: 0,
                secondaryGreen: 0,
                secondaryBlue: 0,
                animation: 0,
                stepDelay: 0
            }
        }
    }

    static readChannelData = (value) => {
        let uInt8Viewer = new Uint8Array(value);
        let uInt16Viewer = new Uint16Array(value);
        return {
            isCentered: uInt8Viewer[0],
            isInterior: uInt8Viewer[1],
            stripUsed: uInt8Viewer[2],
            directionFlipped: uInt8Viewer[3],
            stripType: uInt8Viewer[4],
            stripOrder: uInt8Viewer[5],
            stripPosition: uInt8Viewer[6],
            numLEDs: uInt16Viewer[4], //TODO: Verify if this is correct
            height: uInt16Viewer[5],
            width: uInt16Viewer[6]
        }
    }

    static loadChannelData = async (service, channelIndex) => {
        try{
            let char = await service.getCharacteristic(serviceUUIDs.channelUUIDs[channelIndex]);
            let value = await char.readValue();
            return this.readChannelData(value);
        }catch(error){
            console.log("Error reading channel "+channelIndex+" data: "+error);
            return {
                isCentered: 0,
                isInterior: 0,
                stripUsed: 0,
                directionFlipped: 0,
                stripType: 0,
                stripOrder: 0,
                stripPosition: 0,
                numLEDs: 0,
                height: 0,
                width: 0
            }
        }
    }

    static updateCommand = async (server, primaryColor, secondaryColor, command) => {
        try{
            if(!server.connected){
                server = await server.connect();
            }
            let service = await server.getPrimaryService(serviceUUIDs.serviceUUID);
            let char = await service.getCharacteristic(serviceUUIDs.commandUUID);
    
            let buffer = new ArrayBuffer(2); //underlying buffer for both views
            let buffer16bit = new Uint16Array(buffer);
            buffer16bit[0] = command.delay; //put value in as 16 bit value
            let buffer8bit = new Uint8Array(buffer);
    
            let data = new Uint8Array([
                primaryColor.red,
                primaryColor.green,
                primaryColor.blue,
                secondaryColor.red,
                secondaryColor.green,
                secondaryColor.blue,
                command.animation,
                0, //unknown 0 here for some reason???
                buffer8bit[0], //pull out as 2 8 bit values
                buffer8bit[1]
            ]);
            char.writeValue(data);
            return true;
        }catch(error){
            console.log("Error sending updateCommand ... command: "+error);
            return false;
        }
    }

    static updateChannel = async (server, channelIndex, stripType, numLEDs, position, order, used, isInterior, isCentered, height, width) => {
        try{
            if(!server.connected){
                server = await server.connect();
            }
            let service = await server.getPrimaryService(serviceUUIDs.serviceUUID);
            let char = await service.getCharacteristic(serviceUUIDs.channelUUIDs[channelIndex]);
    
            let buffer = new ArrayBuffer(6); //underlying buffer for both views
            let buffer16bit = new Uint16Array(buffer);
            let buffer8bit = new Uint8Array(buffer);
            buffer16bit[0] = numLEDs; //put value in as 16 bit value
            buffer16bit[1] = height;
            buffer16bit[2] = width;
    
            let data = new Uint8Array([
                isCentered,
                isInterior,
                used,
                stripType,
                order,
                position,
                buffer8bit[0],
                buffer8bit[1],
                buffer8bit[2],
                buffer8bit[3],
                buffer8bit[4],
                buffer8bit[5]
            ]);
            char.writeValue(data);
            return true;
        }catch(error){
            console.log("Error sending updateChannel command: "+error);
            return false;
        }
    }

    static save = async (server) => {
        try{
            if(!server.connected){
                server = await server.connect();
            }
            let service = await server.getPrimaryService(serviceUUIDs.serviceUUID);
            let characteristic = await service.getCharacteristic(serviceUUIDs.saveUUID);
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
            if(!server.connected){
                server = await server.connect();
            }
            let value = (status) ? new Uint8Array([0]) : new Uint8Array([1])
            let service = await server.getPrimaryService(serviceUUIDs.serviceUUID);
            let characteristic = await service.getCharacteristic(serviceUUIDs.onUUID);
            characteristic.writeValue(value);
            return true;
        }catch(error){
            console.log("Error sending toggleLights command: "+error);
            return false;
        }
    }

    static toggleInteriorLights = async (server, status) => {
        try{
            if(!server.connected){
                server = await server.connect();
            }
            let value = (status) ? new Uint8Array([0]) : new Uint8Array([1])
            let service = await server.getPrimaryService(serviceUUIDs.serviceUUID);
            let characteristic = await service.getCharacteristic(serviceUUIDs.interiorOnUUID);
            characteristic.writeValue(value);
            return true;
        }catch(error){
            console.log("Error sending toggleInteriorLights command: "+error);
            return false;
        }
    }
}