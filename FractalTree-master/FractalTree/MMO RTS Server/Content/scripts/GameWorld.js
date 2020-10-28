import Player from "./Player.js";
import Actor from "./Actor.js";

export default class GameWorld {
    constructor() {
        /** @type {Object<number, Player>} */
        this.players = {};
        /** @type {Object<number, Actor>} */
        this.objects = {};
    }

    close() {

    }

    removeObject(id) {
        delete this.objects[id];
    }

    removePlayer(id) {
        delete this.players[id];
    }
}