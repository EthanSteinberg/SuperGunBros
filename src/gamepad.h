//
// Created by Chris Brown on 12/20/16.
//



#ifndef SUPERGUNBROS_GAMEPAD_H
#define SUPERGUNBROS_GAMEPAD_H


#include <string>
#include <map>
#include <memory>

//"Xbox 360 Wired Controller"
//"Logitech Logitech Dual Action"

struct axes;
struct inputs;
typedef unsigned char button_val;

class GamePad {
    //static std::map <std::string, GamePad (*)(int)> mappings;

public:
    //Factory method for gamepads
    static std::shared_ptr<GamePad> getGamePad(int joy);
    //What you actually use to query a gamepad
    virtual inputs getInputs() const = 0;
    //Might not need this, leaving it so as to alter code as little as possible;
    virtual int getIndex() const = 0;
};

//It's nice to have a .x and .y
struct axes{
    float x;
    float y;
};

//Canonical representation of controller inputs
struct inputs{

    axes ls;
    axes rs;
    button_val a, b, x, y;
    button_val lb, rb, lt, rt;
    button_val back, start;
    button_val l3, r3;
    button_val ud, dd, ld, rd;
};


#endif //SUPERGUNBROS_GAMEPAD_H
