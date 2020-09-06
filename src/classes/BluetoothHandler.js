import Animation from './Animation'
import Channel from './Channel'

export default class bluetootHandler {

    constructor(numChannels){
        this.serviceUUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
        this.charUUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";
        this.readBuffer = null;
        this.server = null;
        this.busy = false;
        this.animation = new Animation();

        this.channels = new Array(numChannels)
        for(let i=0; i<numChannels; i++){
            this.channels[i] = new Channel(i);
        }
    }

    connect = async (disconnectListener) => {
        let uuidArray = [];
        uuidArray.push(this.serviceUUID);
        uuidArray.push(this.charUUID);
        let options = {
            acceptAllDevices: true,
            optionalServices: uuidArray,
            /*filters:[
                {services: uuidArray}
            ]*/
        };

        try{
            this.busy = true;
            let device = await navigator.bluetooth.requestDevice(options);
            device.addEventListener('gattserverdisconnected', disconnectListener);
            this.server = await device.gatt.connect();
            if(this.isDisconnected() || this.server.device.name === ""){
                throw new Error("Connection Failed")
            }
            this.busy = false;
        }catch(e){
            this.disconnect();
            this.busy = false;
            throw new Error("Bluetooth Connection: "+e);
        }
    }

    disconnect = () => {
        if(this.isConnected(this.server)){
            this.server.disconnect();
            this.server = null;
        }
    }

    isDisconnected = () => {
        return !this.isConnected();
    }

    isConnected = () => {
        let connected = false;
        if(this.server && this.server.connected){
          connected = true;
        }
        return connected;
    }

    isBusy = () => {
        return this.busy;
    }

    isAvailable = () => {
        return this.isConnected() && !this.isBusy();
    }

    parseBuffer = (data) => {
        //switches are first 6 bytes of data
        let switchesData = data.buffer.slice(0,6);
        let animationData = data.buffer.slice(6,16);
        this.animation.parseBuffer(animationData);

        let channelsData = data.buffer.slice(15);
        this.parseChannels(channelsData);
    }

    getDataBuffer = () => {
        let switchesDataBuffer = new Uint8Array(6);
        let animationDataBuffer = this.animation.getBuffer();
        let combinedBuffer = this.appendBuffer(switchesDataBuffer, animationDataBuffer);
        
        for(let i=0; i<this.channels.length; i++){
            let data = this.channels[i].getBuffer();
            combinedBuffer = this.appendBuffer(combinedBuffer, data);
        }
        return combinedBuffer;
    }

    getChannelsDataBuffer = () => {
        let channelsData = [];

        for(let i=0; i<this.channels.length; i++){
            let data = this.channels[i].getBuffer();
            channelsData = channelsData.concat(data);
        }
        return channelsData;
    }

    write = async () => {
        if(this.isConnected()){
            try{
                this.busy = true;
                let service = await this.server.getPrimaryService(this.serviceUUID);
                let char = await service.getCharacteristic(this.charUUID);
                let dataBuffer = this.getDataBuffer();
                console.log("Write Buffer Length: "+dataBuffer.byteLength);
                await char.writeValue(dataBuffer); //May not need await here
                this.busy = false;
            }catch(error){
                this.disconnect();
                this.busy = false;
                throw new Error("Bluetooth Write Error: "+error);
            }
        }else{
            throw new Error("Bluetooth Disconnected Error");
        }
    }

    read = async () => {
        if(this.isConnected()){
            try{
                this.busy = true;
                let service = await this.server.getPrimaryService(this.serviceUUID);
                let char = await service.getCharacteristic(this.charUUID);
                let data = await char.readValue();
                this.readBuffer = data;
                console.log("Read Buffer Length: "+data.byteLength);
                this.parseBuffer(data);
                this.busy = false;
            }catch(error){
                this.disconnect()
                this.busy = false;
                throw new Error("Bluetooth Read Error: "+error);
            }
        }else{
            throw new Error("Bluetooth Disconnected Error");
        }
    }

    parseChannels = (data) => {
        let channelDataLength = 16;
        let startIndex = 0;
        let endIndex = channelDataLength;
        for(let i=0; i<this.channels.length; i++){
            this.channels[i].parseBuffer(data.slice(startIndex, endIndex+1));
            startIndex = endIndex;
            endIndex = channelDataLength * (i+1);
        }
    }

    appendBuffer = (buffer1, buffer2) => {
        var tmp = new Uint8Array(buffer1.byteLength + buffer2.byteLength);
        tmp.set(new Uint8Array(buffer1), 0);
        tmp.set(new Uint8Array(buffer2), buffer1.byteLength);
        return tmp.buffer;
      };
}