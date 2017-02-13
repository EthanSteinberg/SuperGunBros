#include "readyscreen.h"

#include <GLFW/glfw3.h>
#include <cmath>
#include "gamescreen.h"

#include <algorithm>

const int LEVELS_PER_COLUMN = 3;

ReadyScreen::ReadyScreen(const std::vector<int>& joysticks, unsigned int a_selected_level_index) :
	selected_level_index(a_selected_level_index) {
	for (unsigned int i = 0; i < joysticks.size(); i++) {
		PlayerInfo info;
		info.color = (PlayerColor) i;
		info.joystick_index = joysticks[i];
		players.push_back(info);
		player_ready.push_back(false);

		player_icons.push_back(Player(0, 0, info));
	}

	loaded_levels = Level::load_all_levels();
}

void ReadyScreen::render(RenderList& list) const {
	list.add_image("background", 0, 0);

	list.add_image("ready-header", 10, 10);

	for (unsigned int i = 0; i < players.size(); i++) {
		list.push();
		list.translate(0, 225 + i * 122);
		const PlayerInfo& player = players[i];

		list.add_image("ready-box-outline", 10, 0);
		list.add_image("number-" + get_color_name(player.color), 40, 12);

		{
			list.push();
			list.translate(275, 78);
			player_icons[i].render(list);
			list.pop();
		}

		if (player_ready[i]) {
			list.add_image("ready-text", 400, 50);
		} else {
			list.add_image("not-ready-text", 325, 50);
		}

		list.pop();
	}

	Rectangle level_border(995, 470, 270, 490);
	list.add_rect("white", level_border);
	list.add_outline("black", level_border);

	unsigned int selected_column = selected_level_index / LEVELS_PER_COLUMN;

	for (unsigned int offset = 0; offset < LEVELS_PER_COLUMN; offset++) {
		list.push();
		list.translate(900, 255 + 140 * offset);

		unsigned int level_index = selected_column * LEVELS_PER_COLUMN + offset;
		if (level_index >= loaded_levels.size()){
			list.pop();
			break;
		}
		Level lev = loaded_levels[level_index];

		const double thumbnail_scale = 0.15;

		list.scale(thumbnail_scale, thumbnail_scale);

		if (level_index == selected_level_index) {
			list.add_image("black", -100, -100, 1480, 920);
		}

		const double width_ratio = 1280/lev.width;
		const double height_ratio = 720/lev.height;

		const double level_scale = std::min(width_ratio, height_ratio);

		list.scale(level_scale, level_scale);

		//list.add_image("grey", 0, 0, 1280, 720);
		lev.render(list, false);
		list.pop();
	}

	for (unsigned int column = 0; column < (loaded_levels.size() + LEVELS_PER_COLUMN - 1) / LEVELS_PER_COLUMN; column++) {

		Rectangle column_number(890 + column * 50, 683, 40, 44);
		list.add_rect("grey", column_number);
		if (column == selected_column) {
			list.add_outline("black", column_number);
		}

		std::string number = std::to_string(column + 1);
		Rectangle column_number_text = list.get_image_dimensions(number);
		list.add_rect(number, column_number_text.offset(890 + column * 50, 683));
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

		if (input.buttons[LD] && !last_inputs.at(players[i].joystick_index).buttons[LD]) {
			// This handles the case when the selecte_level_index underflows (it is an unsigned integer)
			selected_level_index = std::min(selected_level_index, selected_level_index - LEVELS_PER_COLUMN);
		}

		if (input.buttons[RD] && !last_inputs.at(players[i].joystick_index).buttons[RD]) {
			selected_level_index = std::min((unsigned int) (loaded_levels.size() - 1), selected_level_index + LEVELS_PER_COLUMN);
		}
	}

	bool all_ready = std::all_of(std::begin(player_ready), std::end(player_ready), [](bool b) {return b;});

	if (all_ready) {
		return std::make_unique<GameScreen>(players, loaded_levels[selected_level_index]);
	}

	return nullptr;
}