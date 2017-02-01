#ifndef MENU_SCREEN_H_INCLUDED
#define MENU_SCREEN_H_INCLUDED

#include "screen.h"

class MenuScreen : public Screen {
public:

	void render(RenderList& list) const;
	std::unique_ptr<Screen> update(const std::map<int, inputs>& joystick_inputs, const std::map<int, inputs>& last_inputs);
private:
};

#endif