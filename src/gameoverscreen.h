#ifndef GAME_OVER_SCREEN_H_INCLUDED
#define GAME_OVER_SCREEN_H_INCLUDED

#include "screen.h"
#include "player.h"

class GameOverScreen : public Screen {
public:
	GameOverScreen(const PlayerInfo& winning_player);

	void render(RenderList& list) const;
	std::unique_ptr<Screen> update(const std::map<int, inputs>& joystick_inputs, const std::map<int, inputs>& last_inputs);

private:
	PlayerInfo winning_player;
    Player player_icon;
};

#endif