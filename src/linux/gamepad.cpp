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
    status.rs.x = natAxes[3];
    status.rs.y = natAxes[4];

    status.buttons[ButtonName::A] = natButtons[0];
    status.buttons[ButtonName::B] = natButtons[1];
    status.buttons[ButtonName::X] = natButtons[2];
    status.buttons[ButtonName::Y] = natButtons[3];

    status.buttons[ButtonName::LB] = natButtons[4];
    status.buttons[ButtonName::RB] = natButtons[5];

    status.buttons[ButtonName::LT] = (button_val) (natAxes[2] > 0);
    status.buttons[ButtonName::RT] = (button_val) (natAxes[5] > 0);

    status.buttons[ButtonName::BACK] = natButtons[6];
    status.buttons[ButtonName::START] = natButtons[7];
    status.buttons[ButtonName::L3] = natButtons[9];
    status.buttons[ButtonName::R3] = natButtons[10];

    status.buttons[ButtonName::UD] = (button_val) (natAxes[7] == -1);
    status.buttons[ButtonName::DD] = (button_val) (natAxes[7] == 1);
    status.buttons[ButtonName::LD] = (button_val) (natAxes[6] == -1);
    status.buttons[ButtonName::RD] = (button_val) (natAxes[6] == 1);


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

    status.buttons[ButtonName::UD] = (button_val) (natAxes[5] == -1);
    status.buttons[ButtonName::DD] = (button_val) (natAxes[5] == 1);
    status.buttons[ButtonName::LD] = (button_val) (natAxes[4] == -1);
    status.buttons[ButtonName::RD] = (button_val) (natAxes[4] == 1);
    return status;
}


inputs getInputs(int joy) {
    std::string jname = glfwGetJoystickName(joy);
    if (jname == "Logitech Logitech Dual Action") {
        return logitech(joy);
    } else if (jname == "Microsoft X-Box 360 pad" || jname == "Xbox 360 Wireless Receiver"){
        return xbox360(joy);
    } else {
        fprintf(stderr, "No gamepad for %s\n", jname.c_str());
        exit(-1);
    }
}


