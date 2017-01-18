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

class Player {
public:
    Player(double start_x, double start_y, PlayerInfo info);
    void render(RenderList& list) const;
    void target_point(double mouseX, double mouseY);
    void update();
    Bullet spawn_bullet() const;

    PlayerInfo info;

    double x;
    double y;

    double dx;
    double dy;

    int health;

    int ticks_till_next_bullet;
    double gun_angle;

    int ticks_left_jumping;
    bool is_jumping;
    bool is_grounded;
    int touching_wall;
private:
    AnimationState get_interpolated_frame() const;
    bool is_facing_right() const;
    std::vector<AnimationState> frames;
    double current_time;
};


#endif //SUPERGUNBROS_PLAYER_H