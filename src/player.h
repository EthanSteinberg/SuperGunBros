//
// Created by Chris Brown on 12/20/16.
//

#ifndef SUPERGUNBROS_PLAYER_H
#define SUPERGUNBROS_PLAYER_H

#include "gamepad.h"

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
    std::shared_ptr<GamePad> gamePad;
};

struct PlayerState {
    double x;
    double y;

    int ticks_left_jumping;

    PlayerInfo info;

    double gun_angle;

    bool facing_right;

    int ticks_till_next_bullet;

    int health;
};



class player {

};


#endif //SUPERGUNBROS_PLAYER_H
