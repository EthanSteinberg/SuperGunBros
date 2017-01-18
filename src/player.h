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

struct PlayerAttributes {
    PlayerType type;
    PlayerColor color;
    int js;
    double width;
    double height;
};

struct PlayerState {
    axes position;
    axes velocity;

    int ticks_left_jumping;

    double gun_angle;

    bool facing_right;
    bool is_jumping;

    int ticks_till_next_bullet;

    int health;
};



class Player {
    PlayerAttributes attr;
    PlayerState state;
protected:
    std::shared_ptr<GamePad> gamepad;
public:
    Player(PlayerAttributes attrib);
    PlayerAttributes getAttributes();
    PlayerState getState();
    inputs getInputs();
    void updateState(PlayerState newState);
    void setPosition(float x, float y);
};

class KeyboardPlayer : public Player {
public:
    KeyboardPlayer(PlayerAttributes attrib, GLFWwindow* window);
};

class KeyboardController : public GamePad {
    GLFWwindow* window;
    Player* player;
public:
    virtual inputs getInputs() const;
    int getIndex() const;
    KeyboardController(Player* p, GLFWwindow* w);
};


#endif //SUPERGUNBROS_PLAYER_H
