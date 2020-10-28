export default class Constants {
    static commandNames = {
        StrafeFoward: 0,
        StrafeBackward: 1,
        StrafeLeft: 2,
        StrafeRight: 3,
        Look: 4,
        Jump: 5,
        Shoot: 6,
        Shoot2: 7,
        Shoot3: 8,
        Shoot4: 9,
        RollLeft: 10,
        RollRight: 11
    };

    static MSG_PREFIX = "QQ";
}

export class MsgTypes {
    static CONNECTION_REQUEST = 1;
    static CONNECTION_ACCEPT = 2;
    static OBJECT_STATE = 3;
    static CLIENT_ACTION = 4;
    static PLAYER_INFO_REQUEST = 5;
    static PLAYER_INFO_RESPONSE = 6;
    static DISCONNECT_REQUEST = 7;
}

export class ActionTypes {
    static COMMAND = 1;
}