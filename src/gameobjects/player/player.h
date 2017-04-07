#ifndef SUPERGUNBROS_PLAYER_H
#define SUPERGUNBROS_PLAYER_H

#include <GLFW/glfw3.h>
#include "gamepad.h"
#include "rendering/renderlist.h"
#include "magic.h"
#include "gameobjects/weapons/bullet.h"
#include "gameobjects/weapons/gun.h"
#include "gameobjects/weapons/flame/flamebullet.h"

#include <iostream>

class Level;

#define MAX_HEALTH 40.0

const int player_width = (int)(90 * ASSET_SCALE);
const int player_height = (int)(200 * ASSET_SCALE);

const int DEATH_TIME = 130;
const int DEATH_ANIMATION_TIME = 30;

const int DEATH_INVISIBLE_TIME = DEATH_TIME - DEATH_ANIMATION_TIME;

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

struct ArmState{
    double needed_angle[2];
    double extra_angle[2];
};

struct PlayerState {

    PlayerState(double x, double y): pos(x, y, player_width, player_height) {}

    Rectangle pos;

    double dx = 0;
    double dy = 0;

    double health = MAX_HEALTH;
    int score = 0;

    int ticks_till_next_bullet = 0;
    int ticks_left_jumping = 0;
    double gun_angle = 0;

    bool jumping = false;
    bool grounded = false;
    int grounded_grace = 0;
    bool roofed = false;
    //-1 left 0 false 1 right
    int pushing_wall = 0;
    int wall_grace = 0;

    bool boosting = false;
    double fuel_left = 1;

    std::unique_ptr<Gun> gun = create_gun("pistol");

    int ammo_left = -1;

    int invincibility_ticks_left = 0;

    bool is_dead = false;
    int ticks_until_spawn = 0;

    int ticks_fire_left = 0;

    int source_fire_player = 0;

    int ticks_till_next_flame_particle = 0;

    bool sticky_shooting = true;
    bool laser_sight = false;

    uint64_t current_player_shooting_sound = 0;
    const char* current_player_shooting_sound_filename = nullptr;

    int ticks_since_last_score_update = -1;

    int last_damager = -1;
};

class Player : public GameObject {
public:
    Player(double start_x, double start_y, PlayerInfo info);

    void render(RenderList& list) const;
    void render_crown(RenderList& list) const;
    void update();

    void set_gun(std::unique_ptr<Gun> gun);

    std::vector<std::unique_ptr<Bullet>> spawn_bullets() const;

    PlayerInfo info;

    PlayerState state;

    void draw_health(RenderList &list) const;

    void draw_laser(RenderList& list, const Level& level) const;

    bool is_facing_right() const;
private:
    Point get_barrel_position() const;

    AnimationState get_interpolated_frame() const;

    double get_gun_angle(double mouseX, double mouseY) const;
    std::vector<AnimationState> frames;
    double current_time;

    double last_time_diff;

    void render_gun(RenderList& list) const;

    void draw_leg(int leg, RenderList& list, AnimationState interpolated) const;
    void draw_arm(int arm, RenderList& list, ArmState arms) const;
};


#endif //SUPERGUNBROS_PLAYER_H