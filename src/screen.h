#ifndef SCREEN_H_INCLUDED
#define SCREEN_H_INCLUDED

#include "renderlist.h"
#include <GLFW/glfw3.h>

class Screen {
public:
	// Render the current screen.
	// Use the provided render list. The mouse is currently at mouseX and mouseY.
	virtual void render(RenderList& list, double mouseX, double mouseY) = 0;

	// Update the state of the game by one tick
	// Return a new Screen if you want to transition, otherwise return nullptr.
	virtual std::unique_ptr<Screen> update(GLFWwindow* window) = 0;

	// A click has occured. Button and action are from GLFW. 
	// See http://www.glfw.org/docs/latest/group__input.html#ga39893a4a7e7c3239c98d29c9e084350c
	virtual std::unique_ptr<Screen> on_click(int button, int action, double mouseX, double mouseY) = 0;

	// A key has been pressed. Key and action are from GLFW.
	// See http://www.glfw.org/docs/latest/group__input.html#ga0192a232a41e4e82948217c8ba94fdfd
	virtual std::unique_ptr<Screen> on_key(int key, int action) = 0;

	virtual ~Screen() {}
};

#endif