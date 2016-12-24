//
// Created by Chris Brown on 12/20/16.
//

#ifndef SUPERGUNBROS_GAMEPAD_H
#define SUPERGUNBROS_GAMEPAD_H


//"Xbox 360 Wired Controller"
//"Logitech Logitech Dual Action"

class GamePad {

public:
    static GamePad* init(int joy);
    virtual const float* getAxes() = 0;
    virtual const unsigned char* getButtons() = 0;
};


#endif //SUPERGUNBROS_GAMEPAD_H
