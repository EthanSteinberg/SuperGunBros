#include "pistolbullet.h"

bool PistolBullet::on_wall_collision(const std::vector<Rectangle>&, std::function<void(int, double)>) {
    return true;
}

bool PistolBullet::on_player_collision(int hit_player, const std::vector<Rectangle>&, std::function<void(int, double)> damage_player) {
    damage_player(hit_player, 10);

    return true;
}

bool PistolBullet::on_no_collision() {
    return false;
}

double PistolBullet::get_velocity() const {
    return 24;
}

const char* PistolBullet::bullet_image_name() const {
    return "bullet";
}