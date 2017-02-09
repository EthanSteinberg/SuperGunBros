#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>

#include <iostream>
#include <vector>

#include "renderlist.h"
#include "render.h"
#include "menuscreen.h"

#include <cmath>

const int screen_width = 1280;
const int screen_height = 720;

const bool debug_keyboard_player = true;

struct MainData {
    std::unique_ptr<Screen> current_screen;
};

void error_callback(int /*error*/, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

void opengl_error(GLenum /*source*/,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei /*length*/,
    const GLchar* message,
    const void* /*userParam*/) {

    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
        return;
    }

    fprintf(stderr, "Opengl Error: %s, %d %d %d\n", message, type, id, severity);
}

void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

inputs keyboard_debug_input(GLFWwindow* window) {
    inputs input;

    input.ls.x = 0;
    input.ls.x -= glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
    input.ls.x += glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;

    input.ls.y = 0;
    input.ls.y -= glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
    input.ls.y += glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;

    //Directional Aiming
    double cursorX;
    double cursorY;
    glfwGetCursorPos(window, &cursorX, &cursorY);

    double needed_gun_angle = atan2(cursorY - 720.0/2, cursorX - 1280.0/2);

    input.rs.x = cos(needed_gun_angle);
    input.rs.y = sin(needed_gun_angle);

    input.buttons[ButtonName::A] = 0;
    input.buttons[ButtonName::B] = 0;
    input.buttons[ButtonName::X] = (button_val) (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS);
    input.buttons[ButtonName::Y] = 0;

    input.buttons[ButtonName::LB] = (button_val) (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS);
    input.buttons[ButtonName::RB] = (button_val) (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);

    input.buttons[ButtonName::LT] = (button_val) (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
    input.buttons[ButtonName::RT] = (button_val)(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);

    input.buttons[ButtonName::BACK] = 0;
    input.buttons[ButtonName::START] = (button_val) (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS);
    input.buttons[ButtonName::L3] = glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS;
    input.buttons[ButtonName::R3] = 0;

    input.buttons[ButtonName::UD] = glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS;
    input.buttons[ButtonName::DD] = glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS;
    input.buttons[ButtonName::LD] = glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS;
    input.buttons[ButtonName::RD] = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;

    return input;
}

const double TIME_PER_TICK = 1.0/60;

std::map<int, inputs> get_joystick_inputs(const std::vector<int>& connected_joysticks, GLFWwindow* window) {
    std::map<int, inputs> joystick_inputs;

    for (int i : connected_joysticks) {
        joystick_inputs[i] = getInputs(i);
    }

    if (debug_keyboard_player) {
        joystick_inputs[-1] = keyboard_debug_input(window);
    }

    return joystick_inputs;
}

int main(void)
{
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    //Window settings
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    //Mac Compatibility
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //Debug mode if compatible
    if (GLAD_GL_KHR_debug) {
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    }

    //Create the window
    GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, "SuperGunBros", nullptr, nullptr);

    //Verify Window Creation
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    if (GLAD_GL_KHR_debug) {
        glDebugMessageCallback(opengl_error, nullptr);
    }

    int pixel_width, pixel_height;
    glfwGetFramebufferSize(window, &pixel_width, &pixel_height);

    create_and_use_program(pixel_width, pixel_height, screen_width, screen_height);

    MainData data;
    data.current_screen = std::make_unique<MenuScreen>();

    glfwSetWindowUserPointer(window, &data);
    glfwSetKeyCallback(window, key_callback);

    double start_time = glfwGetTime();

    std::vector<int> connected_joysticks;

    for (int i = GLFW_JOYSTICK_1; i <= GLFW_JOYSTICK_LAST; i++) {
        if (glfwJoystickPresent(i)) {
            connected_joysticks.push_back(i);
        }
    }

    // The main loop of the program. Just keeps rendering and updating.

    std::map<int, inputs> last_inputs = get_joystick_inputs(connected_joysticks, window);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        RenderList list("../assets/img/pixelPacker.json");

        data.current_screen->render(list);
        list.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();

        double current_time = glfwGetTime();

        while (current_time - start_time > TIME_PER_TICK) {

            std::map<int, inputs> current_inputs = get_joystick_inputs(connected_joysticks, window);

            start_time += TIME_PER_TICK;
            std::unique_ptr<Screen> next_screen = data.current_screen->update(current_inputs, last_inputs);
            if (next_screen) {
                data.current_screen = std::move(next_screen);
            }

            last_inputs = current_inputs;
        }
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

