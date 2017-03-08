#ifndef SCREEN_H_INCLUDED
#define SCREEN_H_INCLUDED

#include "rendering/renderlist.h"
#include <GLFW/glfw3.h>

#include "gamepad.h"
#include <map>

#include <memory>

class SoundThread;

class Screen {
public:
	// Render the current screen.
	// Use the provided render list. The mouse is currently at mouseX and mouseY.
	virtual void render(RenderList& list) const = 0;

	// Update the state of the game by one tick
	// Return a new Screen if you want to transition, otherwise return nullptr.
	virtual std::unique_ptr<Screen> update(const std::map<int, inputs>& joystick_inputs, const std::map<int, inputs>& last_inputs) {
        return nullptr;
    }


    virtual std::unique_ptr<Screen> update(const std::map<int, inputs>& joystick_inputs, const std::map<int, inputs>& last_inputs, SoundThread& sounds) {
        return update(joystick_inputs, last_inputs);
    }

	virtual ~Screen() {}
};

#endif