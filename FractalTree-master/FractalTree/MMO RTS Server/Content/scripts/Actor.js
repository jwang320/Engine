import Vec4 from "./Vec4.js";
import RenderObject from "./RenderObject.js";

export default class Actor {

    /** @type {RenderObject} */
    renderObject = null;

    /**
     * @param {number} globalId
     * @param {number} objectType
     * @param {Vec4} location
     * @param {Vec4} scale
     * @param {Vec4} orientation
     * @param {Vec4} facing
     * @param {Vec4} velocity
     * @param {number} owningPlayer
     */
    constructor(globalId, objectType, location, scale, orientation, facing, velocity, owningPlayer) {
        /** @type {Number} */
        this.globalId = globalId;
        this.objectType = objectType;
        /** @type {Vec4} */
        this.location = location;
        /** @type {Vec4} */
        this.scale = scale;
        /** @type {Vec4} */
        this.orientation = orientation;
        /** @type {Vec4} */
        this.facing = facing;
        /** @type {Vec4} */
        this.velocity = velocity;
        /** @type {number} */
        this.owningPlayer = owningPlayer;
        /** @type {number} */
        this.lastUpdated = new Date().getTime();
    }

    /** @returns {string} */
    toString = () => {
        return "Id: " + this.globalId + ", location: " + this.location.toString() + ", scale: " + this.scale.toString() +
            ", orientation: " + this.orientation.toString() + ", facing: " + this.facing.toString() + ", velocity: " + this.velocity.toString()
            + ", owner: " + this.owningPlayer;
    }

    /**
     * @param {Vec4} location
     * @param {Vec4} scale
     * @param {Vec4} orientation
     * @param {Vec4} facing
     * @param {Vec4} velocity
     * @param {Number} owningPlayer
     */
    update = (location, scale, orientation, facing, velocity, owningPlayer) => {
        this.location = location;
        this.scale = scale;
        this.orientation = orientation;
        this.facing = facing;
        this.velocity = velocity;
        this.owningPlayer = owningPlayer;

        this.lastUpdated = new Date().getTime();
    }

    getFacing = () => {
        const q = this.orientation.tQ;
        const f = this.facing.tVec();
        f.applyQuaternion(q);
        return f;
    }
}