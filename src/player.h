#ifndef SUPERGUNBROS_PLAYER_H
#define SUPERGUNBROS_PLAYER_H

#include "gamepad.h"
#include "renderlist.h"
#include "bullet.h"

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
    std::shared_ptr<GamePad> gamePad;
};

struct AnimationState {
    double hip_angle[2];
    double knee_angle[2];
};

struct Player {

    Player(double start_x, double start_y, PlayerInfo info);

    void render(RenderList& list) const;

    void target_point(double mouseX, double mouseY);

    void update();

    Bullet spawn_bullet() const;

    double x;
    double y;

    double dx;
    double dy;

    int ticks_left_jumping;

    PlayerInfo info;

    double gun_angle;

    bool is_jumping;
    bool double_jump;

    int ticks_till_next_bullet;

    int health;

private:
    AnimationState get_interpolated_frame() const;

    bool is_facing_right() const;


    std::vector<AnimationState> frames;
    double current_time;
};



#endif //SUPERGUNBROS_PLAYER_H
