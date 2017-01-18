//
// Created by Chris Brown on 12/20/16.
//

#include <GLFW/glfw3.h>
#include "player.h"

Player::Player(PlayerAttributes attrib) {
    attr = attrib;
    gamepad = GamePad::getGamePad(attr.js);
    state = {
            {0, 0},
            {0, 0},
            0,
            0,
            true,
            false,
            0,
            100
    };
}

PlayerAttributes Player::getAttributes() {
    return attr;
}

PlayerState Player::getState() {
    return state;
}

inputs Player::getInputs(){
    return gamepad->getInputs();
}

void Player::updateState(PlayerState newState) {
    state = newState;
}

void Player::setPosition(float x, float y) { state.position = {x, y}; }

//Keyboard Player
KeyboardPlayer::KeyboardPlayer(PlayerAttributes attrib, GLFWwindow *win) : Player(attrib) {
    gamepad = (std::shared_ptr<GamePad>)(std::make_shared<KeyboardController>(this, win));
}

KeyboardController::KeyboardController(Player *p, GLFWwindow *w) {
    player = p;
    window = w;
}

int KeyboardController:: getIndex() const {return -1;};

inputs KeyboardController:: getInputs() const {
    inputs status;

    status.ls.x = 0;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        status.ls.x -= 1;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS|| glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        status.ls.x += 1;
    }

    status.ls.y = 0; //TODO whaddup

    //Directional Aiming
    double cursorX;
    double cursorY;
    glfwGetCursorPos(window, &cursorX, &cursorY);
    double xdiff = cursorX - (player->getState().position.x);
    double ydiff = cursorY - (player->getState().position.y);

    status.rs.x = (float)xdiff;
    status.rs.y = (float)ydiff;

    status.a = 0;
    status.b = 0;
    status.x = 0;
    status.y = 0;

    status.lb = 0;
    status.rb = (button_val)(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS);

    status.lt = 0;
    status.rt = (button_val)(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);

    status.back = 0;
    status.start = 0;
    status.l3 = 0;
    status.r3 = 0;

    status.ud = 0;
    status.dd = 0;
    status.ld = 0;
    status.rd = 0;
    return status;

}

