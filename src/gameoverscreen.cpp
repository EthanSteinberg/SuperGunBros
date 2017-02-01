#include "gameoverscreen.h"

#include "readyscreen.h"

GameOverScreen::GameOverScreen(const PlayerInfo& the_winning_player) : winning_player(the_winning_player), player_icon(0, 0, winning_player) {
}

void GameOverScreen::render(RenderList& list) const {

	list.add_image("background", 0, 0);

	list.add_image("game-over-header", (1280 - 714)/2.0, 20);

	list.add_image("player-won-" + get_color_name(winning_player.color), (1280 - 1131)/2, 250);

	{
		list.translate(1280/2.0, 720/2.0 + 50);
		player_icon.render(list);
		list.translate(-1280/2.0, -720/2.0 - 50);
	}


	list.add_image("press-start-continue", (1280 - 1112)/2, 525);
}

std::unique_ptr<Screen> GameOverScreen::update(const std::map<int, inputs>& joystick_inputs, const std::map<int, inputs>& last_inputs) {

	if (!last_inputs.at(winning_player.joystick_index).buttons[START] && joystick_inputs.at(winning_player.joystick_index).buttons[START]) {

		std::vector<int> joysticks;

		for (const auto& item : joystick_inputs) {
			joysticks.push_back(item.first);
		}

		return std::make_unique<ReadyScreen>(joysticks);
	}

	inputs input = joystick_inputs.at(winning_player.joystick_index);

	player_icon.state.dx = input.ls.x * 2;
	player_icon.state.grounded = true;

	if (fabs(input.rs.x) > 0.3 || fabs(input.rs.y) > 0.3) {
	    player_icon.state.gun_angle = atan2(input.rs.y, input.rs.x);
	}

	player_icon.update();

	return nullptr;
}