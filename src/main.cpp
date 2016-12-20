#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>

#include <iostream>
#include <vector>

#include "renderlist.h"
#include "render.h"
#include "menuscreen.h"

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
    } else {
        MainData* data = (MainData*) glfwGetWindowUserPointer(window);
        std::unique_ptr<Screen> next_screen = data->current_screen->on_key(key, action);
        if (next_screen) {
            data->current_screen = std::move(next_screen);
        }
    }
}

void click_callback(GLFWwindow* window, int button, int action, int /*mods*/) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    MainData* data = (MainData*) glfwGetWindowUserPointer(window);
    std::unique_ptr<Screen> next_screen = data->current_screen->on_click(button, action, xpos, ypos);
    if (next_screen) {
        data->current_screen = std::move(next_screen);
    }
}

const double TIME_PER_TICK = 1.0/60;

int main(void)
{
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    if (GLAD_GL_KHR_debug) {
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    }

    GLFWwindow* window = glfwCreateWindow(600, 600, "SuperGunBros", nullptr, nullptr);
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

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    create_and_use_program(width, height);

    MainData data;
    data.current_screen = std::make_unique<MenuScreen>();

    glfwSetWindowUserPointer(window, &data);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, click_callback);

    double start_time = glfwGetTime();

    // The main loop of the program. Just keeps rendering and updating.

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        RenderList list("../assets/img/pixelPacker.json");

        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        data.current_screen->render(list, xpos, ypos);
        list.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();

        double current_time = glfwGetTime();

        while (current_time - start_time > TIME_PER_TICK) {
            start_time += TIME_PER_TICK;
            std::unique_ptr<Screen> next_screen = data.current_screen->update(window);
            if (next_screen) {
                data.current_screen = std::move(next_screen);
            }
        }
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

