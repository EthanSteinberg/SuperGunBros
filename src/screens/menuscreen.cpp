#include "menuscreen.h"

#include "readyscreen.h"

void MenuScreen::render(RenderList& list) const {
	list.add_image("background", 0, 0);
	Rectangle title_rectangle = list.get_image_dimensions("title");
	list.add_rect("title", title_rectangle.offset(1280/2, 200));


	Rectangle start_rectangle = list.get_image_dimensions("start-indicator");
	list.add_rect("start-indicator", start_rectangle.offset(1280/2, 500));
}

std::unique_ptr<Screen> MenuScreen::update(const std::map<int, inputs>& joystick_inputs, const std::map<int, inputs>& last_inputs) {
	std::vector<int> joysticks;

	for (const auto& item : joystick_inputs) {
		joysticks.push_back(item.first);
	}

	for (const auto& item : joystick_inputs) {
		for (int i = 0; i < NUM_BUTTONS; i++) {
			if (item.second.buttons[i] && !last_inputs.at(item.first).buttons[i]) {
				return std::make_unique<ReadyScreen>(joysticks, 0);
			}
		}
	}

	return nullptr;
}