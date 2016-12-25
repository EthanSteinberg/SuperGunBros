//
// Created by Chris Brown on 12/20/16.
//

#include "gamepad.h"

#include <GLFW/glfw3.h>

//#include <map>
//#include <string>

//Mappings of name -> constructor for the associated gamepad class
//std::map <std::string, GamePad (*)(int)> mappings;
//std::string xbx = "Xbox 360 Wired Controller";

class Xbox360Controller: public GamePad{
    int js;
    inputs status;

    //Used to actually query the controller hardware, storing converted results in status
    void update(){
        int count;
        const float* natAxes = glfwGetJoystickAxes(js, &count);
        const unsigned char* natButtons = glfwGetJoystickButtons(js, &count);

        status.ls.x = natAxes[0];
        status.ls.y = natAxes[1];
        status.rs.x = natAxes[2];
        status.rs.y = natAxes[3];

        status.a = natButtons[11];
        status.b = natButtons[12];
        status.x = natButtons[13];
        status.y = natButtons[14];

        status.lb = natButtons[8];
        status.rb = natButtons[9];

        status.lt = (button_val) (natAxes[4] > 0);
        status.rt = (button_val) (natAxes[5] > 0);

        status.back = natButtons[5];
        status.start = natButtons[4];
        status.l3 = natButtons[6];
        status.r3 = natButtons[7];

        status.ud = natButtons[0];
        status.dd = natButtons[1];
        status.ld = natButtons[2];
        status.rd = natButtons[3];

        status.extra = natButtons[10];
    };

public:
    Xbox360Controller(int joy){
        js = joy;
    }

    inputs getState(){
        update();
        return status;
    }

    int getIndex(){return js;}
};

std::shared_ptr<GamePad> GamePad::getGamePad(int joy){
    //The next lines are a zero-fault tolerant version of controller registration
    //const char *jname = glfwGetJoystickName(joy);
    //return mappings[jname](joy);

    return std::make_shared<Xbox360Controller>(joy);
}


