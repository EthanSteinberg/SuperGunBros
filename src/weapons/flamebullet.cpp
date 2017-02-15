#include "flamebullet.h"

#include <cmath>
#include <iostream>

bool FlameBullet::count_down_life() {
    ticks_left -= 1;

    return ticks_left < 0;
}

bool FlameBullet::on_wall_collision(const std::vector<Rectangle>&, std::function<void(int, double)>) {
    return true;
}

bool FlameBullet::on_player_collision(int hit_player, const std::vector<Rectangle>&, std::function<void(int, double)> damage_player) {
    damage_player(hit_player, 0.005);

    return count_down_life();
}

bool FlameBullet::on_no_collision() {
    return count_down_life();
}

double FlameBullet::get_velocity() const {
    return 4;
}

const char* FlameBullet::bullet_image_name() const {
    return "flame-start";
}