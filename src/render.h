#ifndef RENDER_H_INCLUDED
#define RENDER_H_INCLUDED

/**
 * The code here initializes all the appropriate OpenGL status.
 * Right now this code requires a fixed width and height for the entire duration of the program.
 */
void create_and_use_program(int width, int height);

#endif