import BufferIterator from "./BufferIterator.js";
import Constants from "./Constants.js";

export default class Command {
    constructor(name, flags, { arg1 = 0, arg2 = 0, objectId = -1 }) {
        this.name = name;
        this.flags = flags;
        this.arg1 = arg1;
        this.arg2 = arg2;
        this.objectId = objectId;
    }

    /** @param {BufferIterator} buffer */
    serialize(buffer) {
        const actionId = Constants.commandNames[this.name];
        buffer.setInt32(actionId, true);
        buffer.setInt32(this.flags, true);
        buffer.setFloat32(this.arg1, true);
        buffer.setFloat32(this.arg2, true);
        buffer.setInt32(this.objectId, true);
    }
}