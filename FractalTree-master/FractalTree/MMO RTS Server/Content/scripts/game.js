import BufferIterator from "./BufferIterator.js";
import Player from "./Player.js";
import Command from "./Command.js";
import Actor from "./Actor.js";
import Application from "./Application.js";
import Constants, { MsgTypes, ActionTypes } from "./Constants.js";


/** @type {Application} */
let application = null;


document.addEventListener("DOMContentLoaded", (event) => {

    /** @type {WebSocket} */
    let ws;

    const joinButton = document.getElementById("joinButton");
    const accountField = document.getElementById("accountName");


    function requestInitialization() {
        if (accountField.value.length === 0) {
            alert("please enter account name");
            return;
        }

        /** @type {BufferIterator} */
        const buffer = BufferIterator.create(50);
        buffer.setString(Constants.MSG_PREFIX);
        buffer.setInt8(MsgTypes.CONNECTION_REQUEST);
        buffer.setInt8(accountField.value.length);
        buffer.setString(accountField.value);

        ws.send(buffer.buffer);
    }

    /**  @param {number} playerId id of player to get info on*/
    const sendPlayerRequest = playerId => {
        const buffer = BufferIterator.create(7);
        buffer.setString(Constants.MSG_PREFIX);
        buffer.setInt8(MsgTypes.PLAYER_INFO_REQUEST);
        buffer.setInt32(playerId);
        ws.send(buffer.buffer);
    };

    /**
     * @param {number} actionType
     * @param {Command} command
     */
    const sendClientAction = (actionType, command) => {
        const buffer = BufferIterator.create(50);
        buffer.setString(Constants.MSG_PREFIX);
        buffer.setInt8(MsgTypes.CLIENT_ACTION);
        buffer.setInt8(actionType);
        command.serialize(buffer);
        ws.send(buffer.buffer);
    };

    const disconnect = () => {
        const buffer = BufferIterator.create(50);
        buffer.setString(Constants.MSG_PREFIX);
        buffer.setInt8(MsgTypes.DISCONNECT_REQUEST);
        ws.send(buffer.buffer);
    };

    const decodeObject = data => {
        const buffer = new BufferIterator(data);
        const messageType = buffer.getInt8();

        switch (messageType) {
            case MsgTypes.CONNECTION_ACCEPT:
                const clientId = buffer.getInt32();
                application.setClientId(clientId);
                break;
            case MsgTypes.OBJECT_STATE:
                const globalId = buffer.getInt32();
                if (globalId < 0) {
                    break;
                }
                const objectType = buffer.getInt16();
                const location = buffer.getVec4();
                const scale = buffer.getVec4();
                const orientation = buffer.getVec4();
                const facing = buffer.getVec4();
                const velocity = buffer.getVec4();
                const owningPlayer = buffer.getInt32();

                if (!application.game.objects[globalId]) {
                    const obj = new Actor(globalId, objectType, location, scale, orientation, facing, velocity, owningPlayer);
                    application.addObject(obj);
                } else {
                    const obj = application.game.objects[globalId];
                    obj.update(location, scale, orientation, facing, velocity, owningPlayer);
                    application.infoBoard.updateObjectDiv(obj);
                    if (application.game.players[owningPlayer]) {
                        application.game.players[owningPlayer].lastUpdated = new Date().getTime();
                    }
                }

                if (!application.game.players[owningPlayer]) {
                    sendPlayerRequest(owningPlayer);
                }
                break;
            case MsgTypes.PLAYER_INFO_RESPONSE:
                const playerId = buffer.getInt32();
                const len = buffer.getInt32();

                const playerName = buffer.getString(len);
                application.game.players[playerId] = new Player(playerId, playerName);
                application.infoBoard.updatePlayerDiv(application.game.players[playerId]);
                if (application.clientId === playerId) {
                    application.updateLocalPlayer();
                }
                break;
        }
    };

    /**  @param {MessageEvent} evt websocket event*/
    function handleSocketMessage(evt) {
        const msg = evt.data;
        decodeObject(msg);
    }

    function initialize() {
        if (application) {
            return;
        }

        application = new Application();
        application.renderer.renderer.domElement.addEventListener("mousemove", e => {
            if (!application || application.paused) {
                return;
            }
            const x = -e.movementX * .01;
            const y = e.movementY * .01;
            sendClientAction(ActionTypes.COMMAND, new Command("Look", 0, { arg1: x, arg2: y, objectId: application.focusObjectId }));
        });
        if ("WebSocket" in window) {
            ws = new WebSocket("ws://" + window.location.hostname + ":80");
            ws.binaryType = "arraybuffer";
            ws.onopen = requestInitialization;

            ws.onmessage = handleSocketMessage;
            ws.onclose = function () {

            };
        } else {
            // The browser doesn't support WebSocket
            alert("WebSocket NOT supported by your Browser!");
        }
    }

    joinButton.addEventListener("click", (event) => {
        if (accountField.value.length === 0) {
            alert("please enter account name");
            return;
        }
        initialize();
    });

    document.addEventListener("keydown", e => {
        if (!application || application.inputState[e.key]) {
            return;
        }

        application.inputState[e.key] = true;
        console.log(e.key);
        switch (e.key) {
            case "d":
                sendClientAction(ActionTypes.COMMAND, new Command("StrafeLeft", 1, { arg1: 0, arg2: 0, objectId: application.focusObjectId }));
                break;
            case "a":
                sendClientAction(ActionTypes.COMMAND, new Command("StrafeRight", 1, { arg1: 0, arg2: 0, objectId: application.focusObjectId }));
                break;
            case "s":
                sendClientAction(ActionTypes.COMMAND, new Command("StrafeBackward", 1, { arg1: 0, arg2: 0, objectId: application.focusObjectId }));
                break;
            case "w":
                sendClientAction(ActionTypes.COMMAND, new Command("StrafeFoward", 1, { arg1: 0, arg2: 0, objectId: application.focusObjectId }));
                break;
            case "p":
                application.paused = !application.paused;
                break;
            case "ArrowLeft":
                sendClientAction(ActionTypes.COMMAND, new Command("Look", 0, { arg1: -.07, arg2: 0, objectId: application.focusObjectId }));
                break;
            case "ArrowRight":
                sendClientAction(ActionTypes.COMMAND, new Command("Look", 0, { arg1: .07, arg2: 0, objectId: application.focusObjectId }));
                break;
        }
    });

    document.addEventListener("keyup", e => {
        if (!application || !application.inputState[e.key]) {
            return;
        }

        switch (e.key) {
            case "d":
                sendClientAction(ActionTypes.COMMAND, new Command("StrafeLeft", 2, { arg1: 0, arg2: 0, objectId: application.focusObjectId }));
                break;
            case "a":
                sendClientAction(ActionTypes.COMMAND, new Command("StrafeRight", 2, { arg1: 0, arg2: 0, objectId: application.focusObjectId }));
                break;
            case "s":
                sendClientAction(ActionTypes.COMMAND, new Command("StrafeBackward", 2, { arg1: 0, arg2: 0, objectId: application.focusObjectId }));
                break;
            case "w":
                sendClientAction(ActionTypes.COMMAND, new Command("StrafeFoward", 2, { arg1: 0, arg2: 0, objectId: application.focusObjectId }));
                break;
        }

        application.inputState[e.key] = false;
    });

	document.getElementById("testWebSocket").addEventListener("click", event => {
        application.close();
        disconnect();
        ws.close();
        application = null;
    });
});
