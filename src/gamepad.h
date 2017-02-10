#ifndef SUPERGUNBROS_GAMEPAD_H
#define SUPERGUNBROS_GAMEPAD_H

typedef unsigned char button_val;

//It's nice to have a .x and .y
struct axes{
    float x;
    float y;
};

enum ButtonName {
    A = 0,
    B,
    X,
    Y,
    LB,
    RB,
    LT,
    RT,
    BACK,
    START,
    L3,
    R3,
    UD,
    DD,
    LD,
    RD,
    NUM_BUTTONS
};

//Canonical representation of controller inputs
struct inputs {
    axes ls;
    axes rs;
    button_val buttons[NUM_BUTTONS];
};

bool button_press(ButtonName b, inputs current, inputs prev);
bool button_hold(ButtonName b, inputs current, inputs prev);
bool button_release(ButtonName b, inputs current, inputs prev);

inputs getInputs(int joy);

#endif //SUPERGUNBROS_GAMEPAD_H
