export default class Animation {
    constructor(){
        this.animation = 0;
        this.delay = 0;
        this.primaryRed = 0;
        this.primaryGreen = 0;
        this.primaryBlue = 0;
        this.secondaryRed = 0;
        this.secondaryGreen = 0;
        this.secondaryBlue = 0;
        this.animation = 0;
        this.stepDelay = 0;
    }

    parseBuffer(buffer){
        let uInt8Viewer = new Uint8Array(buffer);
        let uInt16Viewer = new Uint16Array(buffer.slice(7, 9));

        this.primaryRed = uInt8Viewer[0];
        this.primaryGreen = uInt8Viewer[1];
        this.primaryBlue = uInt8Viewer[2];
        this.secondaryRed = uInt8Viewer[3];
        this.secondaryGreen = uInt8Viewer[4];
        this.secondaryBlue = uInt8Viewer[5];
        this.animation = uInt8Viewer[6];
        this.stepDelay = uInt16Viewer[0];
    }

    getBuffer(){
        let buffer = new ArrayBuffer(2); //underlying buffer for both views
        let buffer16bit = new Uint16Array(buffer);
        buffer16bit[0] = this.stepDelay; //put value in as 16 bit value
        let buffer8bit = new Uint8Array(buffer);
        let data = new Uint8Array([
            this.primaryRed,
            this.primaryGreen,
            this.primaryBlue,
            this.secondaryRed,
            this.secondaryGreen,
            this.secondaryBlue,
            this.animation,
            0, //unknown 0 here for some reason???
            buffer8bit[0], //pull out as 2 8 bit values
            buffer8bit[1]
        ]);
        return data;
    }
}