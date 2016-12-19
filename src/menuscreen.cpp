#include "menuscreen.h"

#include "readyscreen.h"

void MenuScreen::render(RenderList& list, double mouseX, double mouseY) {
	list.add_image("title", (600 - 529)/2.0, 20);

	if (mouseX > (600 - 529)/2.0 && mouseX < (600 - 529)/2.0 + 529 && mouseY > (600 - 192)/2.0 && mouseY < (600 - 192)/2.0 + 192) {
		list.add_image("startButtonYellow", (600 - 529)/2.0, (600 - 192)/2.0);
	} else {
		list.add_image("startButton", (600 - 529)/2.0, (600 - 192)/2.0);
	}
}

std::unique_ptr<Screen> MenuScreen::update(GLFWwindow* window) {
	return nullptr;
}

std::unique_ptr<Screen> MenuScreen::on_click(int button, int action, double mouseX, double mouseY) {
	if (mouseX > (600 - 529)/2.0 && mouseX < (600 - 529)/2.0 + 529 && mouseY > (600 - 192)/2.0 && mouseY < (600 - 192)/2.0 + 192) {
		return std::make_unique<ReadyScreen>();
	} else {
		return nullptr;
	}
}

std::unique_ptr<Screen> MenuScreen::on_key(int key, int action) {
	return nullptr;
}