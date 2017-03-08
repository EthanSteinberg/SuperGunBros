#ifndef READY_SCREEN_H_INCLUDED
#define READY_SCREEN_H_INCLUDED

#include "screen.h"
#include "gameobjects/player.h"
#include "gameobjects/level/level.h"

class ReadyScreen : public Screen {
public:
	ReadyScreen(const std::vector<int>& joysticks, unsigned int selected_level_index);

	void render(RenderList& list) const;
	std::unique_ptr<Screen> update(const std::map<int, inputs>& joystick_inputs, const std::map<int, inputs>& last_inputs);

private:
	std::vector<PlayerInfo> players;
    std::vector<bool> player_ready;

    std::vector<Player> player_icons;

    std::vector<Level> loaded_levels;
    unsigned int selected_level_index;
};

#endif