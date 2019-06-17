var monitoredCharacteristic;

export default class BlueToothCommands {
    static hm10Service = '0000ffe0-0000-1000-8000-00805f9b34fb';
    static hm10Characteristic = '0000ffe1-0000-1000-8000-00805f9b34fb';
    static getStatusCommand = '?';
    static encoder = new TextEncoder();

    static connect = async (disconnectListener, dataRecievedListener) => {
        let options = {
            //acceptAllDevices: true
            filters:[{
                services: [this.hm10Service]//service exposed by HM-10 module
            }]
        };
        try{
            let device = await navigator.bluetooth.requestDevice(options);
            device.addEventListener('gattserverdisconnected', disconnectListener);
            await device.gatt.connect();
            let service = await device.gatt.getPrimaryService(this.hm10Service);
            monitoredCharacteristic = await service.getCharacteristic(this.hm10Characteristic);
            await monitoredCharacteristic.startNotifications();
            monitoredCharacteristic.addEventListener('characteristicvaluechanged', dataRecievedListener);
            let encodedData = this.encoder.encode(this.getStatusCommand);//question mark is command for arduino to send status
            monitoredCharacteristic.writeValue(encodedData);
            return device;
        }catch(e){
            throw new Error("Bluetooth Connection: "+e);
        }
    }

    static disconnect = (device) => {
        if(this.isConnected(device)){
            device.gatt.disconnect();
        }
    }

    static isConnected = (device) => {
        let connected = false;
        if(device && device.gatt.connected){
          connected = true;
        }
        return connected;
    }

    static writeToDevice = async (device, commandList) => {
        let btCommandsToSend = [];
        btCommandsToSend.push("[");//start character
        btCommandsToSend = btCommandsToSend.concat(this.generateCommandStringsArray(commandList));//array of strings for each bt command
        btCommandsToSend.push("]");//end character

        try{
            for(const command of btCommandsToSend){
                await this.sendBTCommand(device, command);
            }
            return;
        }catch(e){
            throw new Error("Sending BlueTooth Command List: "+e);
        }
    }

    static sendBTCommand = async (device, text) => {
        if(this.isConnected(device)){
            try{
                let encodedData = this.encoder.encode(text);
                await monitoredCharacteristic.writeValue(encodedData);
                return;
            }catch(e){
                throw new Error("Sending BlueTooth Command: "+e);
            }
        }
    }

    static generateCommandStringsArray = (commandList) => {
        var returnArray = [];
        for(let i=0; i<commandList.length; i++){
            let builtString = "";//start with command start character
            if(i > 0){
                builtString += "|"; //add command delimiter
            }
            let red = parseInt(commandList[i].color.substring(1,3), 16);
            let green = parseInt(commandList[i].color.substring(3,5), 16);
            let blue = parseInt(commandList[i].color.substring(5,7), 16);
            builtString += red
            builtString += ",";
            builtString += green
            builtString += ",";
            builtString += blue
            builtString += ",";
            builtString += commandList[i].animation.substring(0,1);
            builtString += ",";
            builtString += commandList[i].delay;
            returnArray.push(builtString);
        }
        return returnArray;
    }
}