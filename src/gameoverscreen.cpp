#include "gameoverscreen.h"

#include "menuscreen.h"

GameOverScreen::GameOverScreen(const PlayerAttributes& the_winning_player) : winning_player(the_winning_player) {
}

void GameOverScreen::render(RenderList& list, double mouseX, double mouseY) {

	const char* winTitle = nullptr;

	switch (winning_player.color) {
		case PlayerColor::RED:
			winTitle = "redWon";
			break;

		case PlayerColor::BLUE:
			winTitle = "blueWon";
			break;

		case PlayerColor::YELLOW:
			winTitle = "yellowWon";
			break;

		case PlayerColor::GREEN:
			winTitle = "greenWon";
			break;
	}

	list.add_image(winTitle, 3, 20);

	if (mouseX > (600 - 529)/2.0 && mouseX < (600 - 529)/2.0 + 529 && mouseY > 400 && mouseY < 400 + 192) {
		list.add_image("backToMenuYellow", (600 - 529)/2.0, 400);
	} else {
		list.add_image("backToMenu", (600 - 529)/2.0, 400);
	}
}

std::unique_ptr<Screen> GameOverScreen::update(GLFWwindow* window) {
	return nullptr;
}

std::unique_ptr<Screen> GameOverScreen::on_click(int button, int action, double mouseX, double mouseY) {
	if (mouseX > (600 - 529)/2.0 && mouseX < (600 - 529)/2.0 + 529 && mouseY > 400 && mouseY < 400 + 192) {
		return std::make_unique<MenuScreen>();
	} else {
		return nullptr;
	}
}

std::unique_ptr<Screen> GameOverScreen::on_key(int key, int action) {
	return nullptr;
}