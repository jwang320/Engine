import Vec4 from './Vec4.js';

export default class BufferIterator {
    /**
     * @param {ArrayBuffer} buffer buffer to read from
     * @param {Number} startOffset optional start offset
     */
    constructor(buffer, startOffset = 0) {
        /** @type {ArrayBuffer} */
        this.buffer = buffer;
        /** @type {number} */
        this.offset = startOffset;

        /** @type {DataView} */
        this.view = new DataView(this.buffer);
    }

    /** @returns {number} a*/
    getInt8 = () => {
        const res = this.view.getInt8(this.offset);
        ++this.offset;
        return res;
    }

    /** @returns {number} a*/
    getInt32 = () => {
        const res = this.view.getInt32(this.offset, true); this.offset += 4;
        return res;
    }

    /** @returns {number} a*/
    getInt16 = () => {
        const res = this.view.getInt16(this.offset, true); this.offset += 2;
        return res;
    }

    /** @returns {Vec4} a*/
    getVec4 = () => {
        const locX = this.view.getFloat32(this.offset, true); this.offset += 4;
        const locY = this.view.getFloat32(this.offset, true); this.offset += 4;
        const locZ = this.view.getFloat32(this.offset, true); this.offset += 4;
        const locW = this.view.getFloat32(this.offset, true); this.offset += 4;

        return new Vec4(locX, locY, locZ, locW);
    }

    /** 
     *  @param {number} length length of string to read
     *  @returns {string} a
     */
    getString = length => {
        const chars = [];
        for (let i = 0; i < length; ++i) {
            chars.push(this.getInt8());
        }

        return String.fromCharCode.apply(String, chars);
    }

    /** @param {number} val a*/
    setInt8 = val => {
        this.view.setInt8(this.offset++, val);
    }

    /** @param {number} val a*/
    setInt32 = val => {
        this.view.setInt32(this.offset, val, true); this.offset += 4;
    }

    /** @param {number} val a*/
    setFloat32 = val => {
        this.view.setFloat32(this.offset, val, true); this.offset += 4;
    }

    /** @param {string} val a*/
    setString = val => {
        for (let i = 0; i < val.length; ++i) {
            this.setInt8(val.charCodeAt(i));
        }
    }

    /** 
     *  @param {number} length a
     *  @returns {BufferIterator} a
     */
    static create = length => {
        return new BufferIterator(new ArrayBuffer(length));
    }
}