#ifndef PLAYER_INFO_H_INCLUDED
#define PLAYER_INFO_H_INCLUDED

enum class PlayerType {
	KEYBOARD,
	GAMEPAD
};

enum class PlayerColor {
	RED,
	BLUE,
	YELLOW,
	GREEN
};

struct PlayerInfo {
	PlayerType type;
	PlayerColor color;
	int joystick_index;
};

#endif