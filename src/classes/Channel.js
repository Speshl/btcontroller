export default class Channel {
    constructor(id){
        this.id = id;
        this.enabled = true;
        this.centered = false;
        this.interior = false;
        this.directionFlipped = false;
        this.leftTurn = false;
        this.rightTurn = false;
        this.brake = false;
        this.reverse = false;
        this.type = 0;
        this.order = 0;
        this.location = 0;
        this.numLEDs = 0;
        this.height = 0;
        this.width = 0;
        this.selected = false; //used for UI only
    }

    getPositionString(numCols){
        let row = Math.floor(this.location / numCols);
        let col = this.location % numCols;
        return row.toString() +":"+col.toString()
    }

    isEnabled(){
        return !(this.isDisabled())
    }

    isDisabled(){
        if(this.type === null || this.type === 0){
            return true;
        }else if(this.numLEDs === null || this.numLEDs <= 0){
            return true;
        }else if(this.order === null || this.order === 0){
            return true;
        }else if(this.enabled === false){
            return true;
        }else if(this.height === null || this.height <= 0){
            return true;
        }else if(this.width === null || this.width <= 0){
            return true;
        }else{
            return false;
        }
    }

    parseBuffer(data){
        let uInt8Viewer = new Uint8Array(data);
        let uInt16Viewer = new Uint16Array(data.slice(11, 17));
        this.enabled = uInt8Viewer[0];
        this.centered = uInt8Viewer[1];
        this.interior = uInt8Viewer[2];
        this.directionFlipped = uInt8Viewer[3];
        this.leftTurn = uInt8Viewer[4];
        this.rightTurn = uInt8Viewer[5];
        this.brake = uInt8Viewer[6];
        this.reverse = uInt8Viewer[7];
        this.type = uInt8Viewer[8];
        this.order = uInt8Viewer[9];
        this.location = uInt8Viewer[10];
        this.numLEDs = uInt16Viewer[0];
        this.height = uInt16Viewer[1];
        this.width = uInt16Viewer[2];
    }

    getBuffer(){
        let buffer = new ArrayBuffer(6); //underlying buffer for both views
        let buffer16bit = new Uint16Array(buffer);
        let buffer8bit = new Uint8Array(buffer);
        buffer16bit[0] = this.numLEDs; //put value in as 16 bit value
        buffer16bit[1] = this.height;
        buffer16bit[2] = this.width;

        let data = new Uint8Array([
            this.enabled,
            this.centered,
            this.interior,
            this.directionFlipped,
            this.leftTurn,
            this.rightTurn,
            this.brake,
            this.reverse,
            this.type,
            this.order,
            this.location,
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