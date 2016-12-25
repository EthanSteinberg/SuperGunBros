//
// Created by Chris Brown on 12/20/16.
//

#include "gamepad.h"

#include <GLFW/glfw3.h>

#include <iostream>

//Mappings of name -> constructor for the associated gamepad class
//std::map <std::string, GamePad (*)(int)> mappings;
//std::string xbx = "Xbox 360 Wired Controller";

class Xbox360Controller: public GamePad {
public:
    Xbox360Controller(int joy) {
        js = joy;
    }

    inputs getState() {
        inputs status;
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
        return status;
    }

    int getIndex(){return js;}

  private:
    int js;
};

class LogitechController: public GamePad {
public:
    LogitechController(int joy) {
        js = joy;
    }

    inputs getState() {
        inputs status;
        int count;
        const float* natAxes = glfwGetJoystickAxes(js, &count);
        const unsigned char* natButtons = glfwGetJoystickButtons(js, &count);

        status.ls.x = natAxes[0];
        status.ls.y = natAxes[1];
        status.rs.x = natAxes[2];
        status.rs.y = natAxes[3];

        status.a = natButtons[1];
        status.b = natButtons[2];
        status.x = natButtons[0];
        status.y = natButtons[3];

        status.lb = natButtons[4];
        status.rb = natButtons[5];

        status.lt = natButtons[6];
        status.rt = natButtons[7];

        status.back = natButtons[8];
        status.start = natButtons[9];
        status.l3 = natButtons[10];
        status.r3 = natButtons[11];

        status.ud = natAxes[5] == -1;
        status.dd = natAxes[5] == 1;
        status.ld = natAxes[4] == -1;
        status.rd = natAxes[4] == 1;
        return status;
    }

    int getIndex(){return js;}

  private:
    int js;
};

std::shared_ptr<GamePad> GamePad::getGamePad(int joy){
    std::string jname = glfwGetJoystickName(joy);
    if (jname == "Logitech Logitech Dual Action") {
        return std::make_shared<LogitechController>(joy);
    } else {
        return std::make_shared<Xbox360Controller>(joy);
    }
}


