#ifndef RENDER_H_INCLUDED
#define RENDER_H_INCLUDED

#include <array>
#include <functional>

/**
 * The code here initializes all the appropriate OpenGL status.
 * Right now this code requires a fixed width and height for the entire duration of the program.
 */
std::array<std::function<void(void)>, 2> create_and_use_program(int pixel_width, int pixel_height, int screen_width, int screen_height);

#endif