#ifndef SUPERGUNBROS_PLAYER_H
#define SUPERGUNBROS_PLAYER_H

#include <GLFW/glfw3.h>
#include "gamepad.h"
#include "renderlist.h"
#include "bullet.h"

#define MAX_HEALTH 40.0

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

enum class PlayerWeapon {
    PISTOL,
    ROCKET,
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

    inputs input;

    PlayerWeapon current_weapon = PlayerWeapon::PISTOL;

    int ammo_left = -1;
};

class Player {
public:
    Player(double start_x, double start_y, PlayerInfo info);
    void render(RenderList& list) const;
    void update(GLFWwindow* window);
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

    double gun_rotation_x() const;
    double gun_rotation_y() const;

    double gun_offset_x() const;
    double gun_offset_y() const;

    double barrel_offset_x() const;
    double barrel_offset_y() const;

    const char* gun_image_name() const;

    bool gun_in_front() const;

    void render_gun(RenderList& list) const;
};


#endif //SUPERGUNBROS_PLAYER_H