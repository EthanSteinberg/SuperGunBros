#ifndef MENU_SCREEN_H_INCLUDED
#define MENU_SCREEN_H_INCLUDED

#include "screen.h"

class MenuScreen : public Screen {
public:
	void render(RenderList& list, double mouseX, double mouseY);
	std::unique_ptr<Screen> update(GLFWwindow* window);
	std::unique_ptr<Screen> on_click(int button, int action, double mouseX, double mouseY);
	std::unique_ptr<Screen> on_key(int key, int action);

private:
};

#endif