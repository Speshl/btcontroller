class Channel {
    constructor(id){
        this.id = id;
        this.type = null,
        this.numLEDs = null,
        this.position = 0;
        this.order = null;
        this.used = false;
        this.isInterior = false;
        this.isCentered = false;
        this.height = null;
        this.width = null;
        this.turnEnabled = false;
        this.breakEnabled = false;
        this.channelSelected = false;
    }

    getPositionString(){
        let numCols = 3;
        let row = Math.floor(this.position / numCols);
        let col = this.position % numCols;
        return row.toString() +":"+col.toString()
    }

    isEnabled(){
        return !(this.isDisabled())
    }

    isDisabled(){
        if(this.type === null){
            return true;
        }else if(this.numLEDs === null || this.numLEDs <= 0){
            return true;
        }else if(this.order === null){
            return true;
        }else if(this.used === false){
            return true;
        }else if(this.height === null || this.height <= 0){
            return true;
        }else if(this.width === null || this.width <= 0){
            return true;
        }else{
            return false;
        }
    }

    populateFromBuffer(buffer){
        let uInt8Viewer = new Uint8Array(buffer);
        let uInt16Viewer = new Uint16Array(buffer);
        this.isCentered = uInt8Viewer[0];
        this.isInterior = uInt8Viewer[1];
        this.stripUsed = uInt8Viewer[2];
        this.stripType = uInt8Viewer[3];
        this.stripOrder = uInt8Viewer[4];
        this.stripPosition = uInt8Viewer[5];
        this.numLEDs = uInt16Viewer[3]; //TODO: Verify if this is correct
        this.height = uInt16Viewer[4];
        this.width = uInt16Viewer[5];
    }

    getBufferArray(){
        let buffer = new ArrayBuffer(6); //underlying buffer for both views
        let buffer16bit = new Uint16Array(buffer);
        let buffer8bit = new Uint8Array(buffer);
        buffer16bit[0] = this.numLEDs; //put value in as 16 bit value
        buffer16bit[1] = this.height;
        buffer16bit[2] = this.width;

        let data = new Uint8Array([
            this.isCentered,
            this.isInterior,
            this.used,
            this.stripType,
            this.order,
            this.position,
            buffer8bit[0],
            buffer8bit[1],
            buffer8bit[2],
            buffer8bit[3],
            buffer8bit[4],
            buffer8bit[5]
        ]);
        return data;
    }
}