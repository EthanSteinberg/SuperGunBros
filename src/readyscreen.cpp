#include "readyscreen.h"

#include <GLFW/glfw3.h>
#include "gamescreen.h"

ReadyScreen::ReadyScreen() {
	PlayerInfo keyboardPlayer = {
			PlayerType::KEYBOARD,
			PlayerColor::RED,
			NULL //TODO: build keyboard gamepad abstraction
	};

	players.push_back(keyboardPlayer);
}

void ReadyScreen::render(RenderList& list, double mouseX, double mouseY) {
	list.add_image("selectPlayers", (600 - 529)/2.0, 20);


	if (mouseX > (600 - 529)/2.0 && mouseX < (600 - 529)/2.0 + 529 && mouseY > 400 && mouseY < 400 + 192) {
		list.add_image("startButtonYellow", (600 - 529)/2.0, 400);
	} else {
		list.add_image("startButton", (600 - 529)/2.0, 400);
	}

	for (unsigned int i = 0; i < players.size(); i++) {
		const PlayerInfo& player = players[i];

		const char* type = nullptr;
		const char* color = nullptr;

		switch (player.type) {
			case PlayerType::KEYBOARD:
				type = "keyboard";
				break;

			case PlayerType::GAMEPAD:
				type = "gamepad";
				break;
		}

		switch (player.color) {
			case PlayerColor::RED:
				color = "red";
				break;

			case PlayerColor::BLUE:
				color = "blue";
				break;

			case PlayerColor::YELLOW:
				color = "yellow";
				break;

			case PlayerColor::GREEN:
				color = "green";
				break;
		}

		list.add_image(type, 20, 100 + i * 100);
		list.add_image(color, 320, 100 + i * 100, 100, 100);
	}
}

std::unique_ptr<Screen> ReadyScreen::update(GLFWwindow* window) {
	for (int index = 0; index < GLFW_JOYSTICK_LAST; index++) {
		if (glfwJoystickPresent(index) && !contains_player_for_joystick(index)) {
			int count;
			const unsigned char* axes = glfwGetJoystickButtons(index, &count);

			for (int i = 0; i < count ; i++) {
				if (axes[i]) {
					PlayerInfo joystickPlayer = {
							PlayerType::GAMEPAD,
							PlayerColor::YELLOW,
							GamePad::getGamePad(index)
					};

					printf("JS: %d, name: %s\n", index, glfwGetJoystickName(index));

					if (players.size() == 1) {
						players.push_back(joystickPlayer);
					} else {
						joystickPlayer.color = PlayerColor::RED;
						players[0] = joystickPlayer;
					}


					break;
				}
			}
		}
	}

	return nullptr;
}

std::unique_ptr<Screen> ReadyScreen::on_click(int button, int action, double mouseX, double mouseY) {
	if (mouseX > (600 - 529)/2.0 && mouseX < (600 - 529)/2.0 + 529 && mouseY > 400 && mouseY < 400 + 192) {
		return std::make_unique<GameScreen>(players);
	} else {
		return nullptr;
	}
}

std::unique_ptr<Screen> ReadyScreen::on_key(int key, int action) {
	return nullptr;
}

bool ReadyScreen::contains_player_for_joystick(int index) {
	for (const PlayerInfo& info : players) {

		if (info.gamePad && info.gamePad->getIndex() == index) {
			return true;
		}
	}

	return false;
}