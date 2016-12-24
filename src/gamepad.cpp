//
// Created by Chris Brown on 12/20/16.
//

#include "gamepad.h"

#include <GLFW/glfw3.h>
#include <map>
#include <string>

//This would, in theory, be used to store all the various name -> GamePad constructors
std::map <std::string, GamePad (*)(int)> mappings;

class Xbox360Controller: public GamePad{
    int js;

public:
    Xbox360Controller(int joy){
        js = joy;
    }

    const float* getAxes(){
        int count;
        const float* natAxes = glfwGetJoystickAxes(js, &count);
        return natAxes;
    }

    const unsigned char* getButtons(){
        int count;
        //const float* natAxes = glfwGetJoystickAxes(js, &count);
        const unsigned char* natButtons = glfwGetJoystickButtons(js, &count);
        return natButtons;
    }
};

GamePad* GamePad::init(int joy){
    //  The next lines are a zero-fault tolerant version of what should happen when we have more mappings
    //const char *jname = glfwGetJoystickName(joy);
    // return mappings[jname](joy);
    return new Xbox360Controller(joy);
}
