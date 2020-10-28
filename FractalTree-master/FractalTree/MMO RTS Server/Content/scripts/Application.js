import Player from "./Player.js";
import GameWorld from "./GameWorld.js";
import Actor from "./Actor.js";
import InfoBoard from "./InfoBoard.js";
import Renderer from "./Renderer.js";
import RenderObject from "./RenderObject.js";

export default class Application {
    /** @type {Player} */
    localPlayer = null;

    /** @type {number} */
    clientId = null;

    /** @type {number} */
    focusObjectId = null;

    /** @type {Object<string, boolean>} */
    inputState = {};

    /** @type {boolean} */
    paused = true;

    constructor() {
        /** @type {GameWorld} */
        this.game = new GameWorld();

        /** @type {InfoBoard} */
        this.infoBoard = new InfoBoard();

        /** @type {Renderer} */
        this.renderer = new Renderer(this.infoBoard, this);

    }

    /** @param {number} clientId id of this player*/
    setClientId(clientId) {
        this.clientId = clientId;
    }

    updateLocalPlayer() {
        this.localPlayer = this.game.players[this.clientId];
        this.infoBoard.updatePlayerInfoDiv(this.localPlayer);
    }

    /** @param {number} id a*/
    setFocusObjectId(id) {
        this.focusObjectId = id;
        this.renderer.focusObjectId = id;
        this.infoBoard.updateFocusObjectIdDiv(id);
    }

    close() {
        this.renderer.close();
        this.infoBoard.close();
    }

    /** @param {Actor} obj a*/
    addObject(obj) {
        this.game.objects[obj.globalId] = obj;
        this.infoBoard.createDiv(obj.globalId);
        this.infoBoard.updateObjectDiv(obj);
        this.renderer.addObject(new RenderObject(obj));

        if (obj.objectType === 2 && obj.owningPlayer === this.clientId) {
            this.setFocusObjectId(obj.globalId);
        }
    }

    /** @param {Actor} obj a*/
    removeObject(obj) {
        this.infoBoard.deleteObjectDiv(obj);
        this.renderer.removeObject(obj.renderObject);
        this.game.removeObject(obj.globalId);
    }

    /** @param {Player} player a*/
    removePlayer(player) {
        this.infoBoard.deletePlayerDiv(player);
        this.game.removePlayer(player.id);
    }

    clearStaleObjects() {
        const currentTime = new Date().getTime();
        for (let id in this.game.objects) {
            if (this.game.objects.hasOwnProperty(id)) {
                const updateTime = this.game.objects[id].lastUpdated;

                if (currentTime - updateTime > 5000) {
                    this.removeObject(this.game.objects[id]);
                }
            }
        }

        this.clearStalePlayers();
    }

    clearStalePlayers() {
        const currentTime = new Date().getTime();
        for (let id in this.game.players) {
            if (this.game.players.hasOwnProperty(id)) {
                const updateTime = this.game.players[id].lastUpdated;

                if (currentTime - updateTime > 5000) {
                    this.removePlayer(this.game.players[id]);
                }
            }
        }
    }
}