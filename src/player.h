//
// Created by Chris Brown on 12/20/16.
//

#ifndef SUPERGUNBROS_PLAYER_H
#define SUPERGUNBROS_PLAYER_H

enum class PlayerType {
    KEYBOARD,
    GAMEPAD
};

enum class PlayerColor {
    RED,
    BLUE,
    YELLOW,
    GREEN
};

struct PlayerInfo {
    PlayerType type;
    PlayerColor color;
    int joystick_index;
};



class player {

};


#endif //SUPERGUNBROS_PLAYER_H
