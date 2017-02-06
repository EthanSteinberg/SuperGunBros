#include "readyscreen.h"

#include <GLFW/glfw3.h>
#include <cmath>
#include "gamescreen.h"

const char* level_names[] = {
	"../assets/level/level_1.json",
	"../assets/level/level_2.json",
};

ReadyScreen::ReadyScreen(const std::vector<int>& joysticks) {
	for (unsigned int i = 0; i < joysticks.size(); i++) {
		PlayerInfo info;
		info.color = (PlayerColor) i;
		info.joystick_index = joysticks[i];
		players.push_back(info);
		player_ready.push_back(false);

		player_icons.push_back(Player(0, 0, info));
	}

	for (const auto& level_name : level_names) {
		loaded_levels.push_back(Level::load_from_file(level_name));
	}
}

const double level_scale = 0.15;

void ReadyScreen::render(RenderList& list) const {
	list.add_image("background", 0, 0);

	list.add_image("ready-header", 10, 10);

	for (unsigned int i = 0; i < players.size(); i++) {
		list.translate(0, 275 + i * 140);
		const PlayerInfo& player = players[i];

		list.add_image("ready-box-outline", 10, 0);
		list.add_image("number-" + get_color_name(player.color), 40, 12);

		{
			list.translate(275, 70);
			player_icons[i].render(list);
			list.translate(-275, -70);
		}

		if (player_ready[i]) {
			list.add_image("ready-text", 400, 50);
		} else {
			list.add_image("not-ready-text", 325, 50);
		}

		list.translate(0, -(275 + i * 140.0));
	}

	for (unsigned int i = 0; i < loaded_levels.size(); i++) {
		list.translate(900, 300 + 150 * i);

		list.scale(level_scale, level_scale);

		if (i == selected_level_index) {
			list.add_image("black", -100, -100, 1480, 920);
		}

		list.add_image("grey", 0, 0, 1280, 720);
		loaded_levels[i].render(list, false);

		list.scale(1/level_scale, 1/level_scale);

		list.translate(-900, -(300 + 150.0 * i));
	}
}

std::unique_ptr<Screen> ReadyScreen::update(const std::map<int, inputs>& joystick_inputs, const std::map<int, inputs>& last_inputs) {

	for (unsigned int i = 0; i < players.size(); i++) {

		Player& player = player_icons[i];

		inputs input = joystick_inputs.at(players[i].joystick_index);

		float x_thresh = (fabs(input.ls.x) > 0.3) ? input.ls.x : 0;

		player.state.dx = x_thresh * 2;
		player.state.grounded = true;

		if (fabs(input.rs.x) > 0.3 || fabs(input.rs.y) > 0.3) {
		    player.state.gun_angle = atan2(input.rs.y, input.rs.x);
		}

		player.update();

		if (input.buttons[START] && !last_inputs.at(players[i].joystick_index).buttons[START]) {
			player_ready[i] = true;
		}

		if (input.buttons[UD] && !last_inputs.at(players[i].joystick_index).buttons[UD]) {
			// This handles the case when the selecte_level_index underflows (it is an unsigned integer)
			selected_level_index = std::min(selected_level_index, selected_level_index - 1);
		}

		if (input.buttons[DD] && !last_inputs.at(players[i].joystick_index).buttons[DD]) {
			selected_level_index = std::min((unsigned int) (loaded_levels.size() - 1), selected_level_index + 1);
		}
	}

	bool all_ready = std::all_of(std::begin(player_ready), std::end(player_ready), [](bool b) {return b;});

	if (all_ready) {
		return std::make_unique<GameScreen>(players, loaded_levels[selected_level_index]);
	}

	return nullptr;
}