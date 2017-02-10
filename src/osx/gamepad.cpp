#include "../gamepad.h"

#include <GLFW/glfw3.h>

#include <iostream>

static inputs xbox360(int joy) {
    inputs status;
    int count;
    const float* natAxes = glfwGetJoystickAxes(joy, &count);
    const unsigned char* natButtons = glfwGetJoystickButtons(joy, &count);

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

static inputs logitech(int joy) {

    inputs status;
    int count;
    const float* natAxes = glfwGetJoystickAxes(joy, &count);
    const unsigned char* natButtons = glfwGetJoystickButtons(joy, &count);

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

inputs getInputs(int joy) {
    //if(joy == -1) {return nullptr;} //TODO fix for keyboards
    std::string jname = glfwGetJoystickName(joy);
    if (jname == "Logitech Dual Action") {
        return logitech(joy);
    } else if (jname == "Xbox 360 Wired Controller" || jname == "Wireless 360 Controller"){
        return xbox360(joy);
    } else {
        fprintf(stderr, "No gamepad for %s\n", jname.c_str());
        exit(-1);
    }
}

bool button_press(ButtonName b, inputs current, inputs prev) {
    return current.buttons[b] && !prev.buttons[b];
}

bool button_hold(ButtonName b, inputs current, inputs prev) {
    return current.buttons[b] && prev.buttons[b];
}

bool button_release(ButtonName b, inputs current, inputs prev) {
    return !current.buttons[b] && prev.buttons[b];
}