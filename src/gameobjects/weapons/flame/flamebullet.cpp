#include "flamebullet.h"

#include <cmath>
#include <iostream>

FlameBullet::FlameBullet(bool a_individual) : individual(a_individual) {}

bool FlameBullet::count_down_life() {
    ticks_left -= 1;
    ticks_alive += 1;

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
    double fraction = std::pow(1 - (ticks_alive / MAX_LIFE), 2.0);
    return 4 * fraction + 6;
}

const char* FlameBullet::bullet_image_name() const {
    return "flame-start";
}

void FlameBullet::render(RenderList& list) const {

    double yellow = 1 - std::pow(1 - (ticks_left / MAX_LIFE), 2.0);

    double alpha = individual ? 0.75 : 0.5;

    list.push();
    list.set_z(30);
    list.add_flame(pos.x, pos.y, 1, yellow, 0, alpha);
    list.pop();
}

bool FlameBullet::catch_on_fire() const {
    return true;
}