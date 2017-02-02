#ifndef SUPERGUNBROS_PLAYER_H
#define SUPERGUNBROS_PLAYER_H

#include <GLFW/glfw3.h>
#include "gamepad.h"
#include "renderlist.h"
#include "bullet.h"
#include "gun.h"
#include "pistol.h"
#include "rocket.h"

#define MAX_HEALTH 40.0

enum class PlayerColor {
    RED = 0,
    BLUE,
    YELLOW,
    GREEN
};

inline std::string get_color_name(PlayerColor color) {
    switch (color) {
        case PlayerColor::RED:
            return "red";

        case PlayerColor::BLUE:
            return "blue";

        case PlayerColor::YELLOW:
            return "yellow";

        case PlayerColor::GREEN:
            return "green";

        default:
            std::cout<<"Invalid color " << (int)color << std::endl;
            exit(-1);
    }
}

struct PlayerInfo {
    PlayerColor color;
    int joystick_index;
};

struct AnimationState {
    double hip_angle[2];
    double knee_angle[2];
};

struct PlayerState {

    PlayerState(double x, double y): pos(x, y, 35, 72) {}

    Rectangle pos;

    double dx = 0;
    double dy = 0;

    int health = MAX_HEALTH;
    int lives_left = 3;

    int ticks_till_next_bullet = 0;
    int ticks_left_jumping = 0;
    double gun_angle = 0;

    bool jumping = false;
    bool grounded = false;
    bool roofed = false;
    //-1 left 0 false 1 right
    int pushing_wall = 0;

    bool boosting = false;
    double fuel_left = 1;

    std::shared_ptr<Gun> gun = std::make_shared<Pistol>();

    int ammo_left = -1;

    int invincibility_ticks_left = 0;
};

class Player {
public:
    Player(double start_x, double start_y, PlayerInfo info);
    void render(RenderList& list) const;
    void update();
    Bullet spawn_bullet() const;

    PlayerInfo info;

    PlayerState state;
private:
    AnimationState get_interpolated_frame() const;

    double get_gun_angle(double mouseX, double mouseY) const;

    bool is_facing_right() const;
    std::vector<AnimationState> frames;
    double current_time;

    double last_time_diff;

    void render_gun(RenderList& list) const;
};


#endif //SUPERGUNBROS_PLAYER_H