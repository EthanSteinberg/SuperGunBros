//
// Created by Chris Brown on 12/20/16.
//

#include "../gamepad.h"

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

    inputs getInputs() const {
        inputs status;
        int count;
        const float* natAxes = glfwGetJoystickAxes(js, &count);
        const unsigned char* natButtons = glfwGetJoystickButtons(js, &count);

        status.ls.x = natAxes[0];
        status.ls.y = natAxes[1];
        status.rs.x = natAxes[2];
        status.rs.y = natAxes[3];

        status.buttons[ButtonName::A] = natButtons[11];
        status.buttons[ButtonName::B] = natButtons[12];
        status.buttons[ButtonName::X] = natButtons[13];
        status.buttons[ButtonName::Y] = natButtons[14];

        status.buttons[ButtonName::LB] = natButtons[8];
        status.buttons[ButtonName::RB] = natButtons[9];

        status.buttons[ButtonName::LT] = (button_val) (natAxes[4] > 0);
        status.buttons[ButtonName::RT] = (button_val) (natAxes[5] > 0);

        status.buttons[ButtonName::BACK] = natButtons[5];
        status.buttons[ButtonName::START] = natButtons[4];
        status.buttons[ButtonName::L3] = natButtons[6];
        status.buttons[ButtonName::R3] = natButtons[7];

        status.buttons[ButtonName::UD] = natButtons[0];
        status.buttons[ButtonName::DD] = natButtons[1];
        status.buttons[ButtonName::LD] = natButtons[2];
        status.buttons[ButtonName::RD] = natButtons[3];

        //natButtons[10] is the xbox button

        return status;
    }

    int getIndex() const {return js;}

  private:
    int js;
};

class LogitechController: public GamePad {
public:
    LogitechController(int joy) {
        js = joy;
    }

    inputs getInputs() const {

        inputs status;
        int count;
        const float* natAxes = glfwGetJoystickAxes(js, &count);
        const unsigned char* natButtons = glfwGetJoystickButtons(js, &count);

        status.ls.x = natAxes[0];
        status.ls.y = natAxes[1];
        status.rs.x = natAxes[2];
        status.rs.y = natAxes[3];

        status.buttons[ButtonName::A] = natButtons[1];
        status.buttons[ButtonName::B] = natButtons[2];
        status.buttons[ButtonName::X] = natButtons[0];
        status.buttons[ButtonName::Y] = natButtons[3];

        status.buttons[ButtonName::LB] = natButtons[4];
        status.buttons[ButtonName::RB] = natButtons[5];

        status.buttons[ButtonName::LT] = natButtons[6];
        status.buttons[ButtonName::RT] = natButtons[7];

        status.buttons[ButtonName::BACK] = natButtons[8];
        status.buttons[ButtonName::START] = natButtons[9];

        status.buttons[ButtonName::L3] = natButtons[10];
        status.buttons[ButtonName::R3] = natButtons[11];

        status.buttons[ButtonName::UD] = natButtons[12];
        status.buttons[ButtonName::DD] = natButtons[14];
        status.buttons[ButtonName::LD] = natButtons[15];
        status.buttons[ButtonName::RD] = natButtons[13];
        return status;
    }

    int getIndex() const {return js;}

  private:
    int js;
};

std::shared_ptr<GamePad> GamePad::getGamePad(int joy){
    if(joy == -1) {return nullptr;} //TODO fix for keyboards
    std::string jname = glfwGetJoystickName(joy);
    if (jname == "Logitech Dual Action") {
        return std::make_shared<LogitechController>(joy);
    } else if (jname == "Xbox 360 Wired Controller"){
        return std::make_shared<Xbox360Controller>(joy);
    } else {
        fprintf(stderr, "No gamepad for %s\n", jname.c_str());
        exit(-1);
    }
}


