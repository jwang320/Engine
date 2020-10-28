import Vec4 from "./Vec4.js";
import Actor from "./Actor.js";
import Player from "./Player.js";
import Utils from "./Utils.js";

export default class InfoBoard {
    constructor() {
        /** @type {HTMLElement} */
        this.objDiv = document.getElementById("objectList");
        /** @type {HTMLElement} */
        this.playerInfoDiv = document.getElementById("playerList");
        /** @type {HTMLElement} */
        this.localPlayerInfoDiv = document.getElementById("playerInfo");
        /** @type {HTMLElement} */
        this.focusObjectIdDiv = document.getElementById("focusObjectId");
        /** @type {HTMLElement} */
        this.cameraPositionDiv = document.getElementById("cameraPosition");
    }

    /** @param {Number} globalId id*/
    createDiv = (globalId) => {
        const elem = document.createElement('div');
        elem.id = "object_div_" + globalId;
        this.objDiv.appendChild(elem);
    };

    /** @param {Actor} obj object */
    updateObjectDiv = (obj) => {
        const elem = document.getElementById("object_div_" + obj.globalId);
        elem.innerHTML = obj.toString();
    };

    /** @param {Actor} obj a*/
    deleteObjectDiv = (obj) => {
        this.objDiv.removeChild(document.getElementById("object_div_" + obj.globalId));
    }

    /** @param {Player} player a*/
    deletePlayerDiv = (player) => {
        this.playerInfoDiv.removeChild(document.getElementById("player_div_" + player.id));
    }

    /** @param {Player} player p */
    updatePlayerDiv = player => {
        let elem = document.getElementById("player_div_" + player.id);
        if (!elem) {
            elem = document.createElement('div');
            elem.id = "player_div_" + player.id;
            this.playerInfoDiv.appendChild(elem);
        }

        elem.innerHTML = player.toString();
    };

    /** @param {number} focusObjectId a*/
    updateFocusObjectIdDiv = focusObjectId => {
        this.focusObjectIdDiv.innerHTML = "Focus object id: " + focusObjectId;//localPlayer.toString();
    }

    /** @param {Player} localPlayer a*/
    updatePlayerInfoDiv = localPlayer => {
        this.localPlayerInfoDiv.innerHTML = "Your player info is: " + localPlayer.toString();//localPlayer.toString();
    }

    /** @param {Vec4} pos a*/
    updateCameraPosition = pos => {
        this.cameraPositionDiv.innerHTML = "Camera position: " + pos.toString();
    }

    close() {
        Utils.deleteChildren(this.playerInfoDiv);
        Utils.deleteChildren(this.objDiv);

        this.focusObjectIdDiv.innerHTML = "";
        this.localPlayerInfoDiv.innerHTML = "";
        this.cameraPositionDiv.innerHTML = "";
    }
}