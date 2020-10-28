export default class Vec4 {
    constructor(x, y, z, w) {
        /** @type {Number} */
        this.x = x;
        /** @type {Number} */
        this.y = y;
        /** @type {Number} */
        this.z = z;
        /** @type {Number} */
        this.w = w;
    }

    /** @returns {string} a*/
    toString = () => {
        return "{x: " + this.x + ", y: " + this.y + ", z: " + this.z + ", w: " + this.w + "}";
    }

    /** @returns {THREE.Vector3} a */
    tVec = () => {
        return new THREE.Vector3(this.x, this.y, this.z);
    }

    /** @returns {THREE.Quaternion} a */
    get tQ() {
        return new THREE.Quaternion(this.x, this.y, this.z, this.w);
    }
}