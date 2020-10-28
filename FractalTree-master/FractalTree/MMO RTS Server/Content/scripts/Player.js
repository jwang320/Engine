export default class Player {
    /**
     * @param {number} id eslint won't stop complaining unless description
     * @param {string} name eslint won't stop complaining unless description
     */
    constructor(id, name) {
        /** @type {number} */
        this.id = id;
        /** @type {string} */
        this.name = name;
        /** @type {number} */
        this.focusObjectId = null;

        this.lastUpdated = new Date().getTime();
    }

    /** @returns {string} to string*/
    toString = () => {
        return "id: " + this.id + ", name: " + this.name;
    }
}