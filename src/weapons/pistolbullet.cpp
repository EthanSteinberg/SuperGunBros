#include "pistolbullet.h"

bool PistolBullet::on_wall_collision(const std::vector<Rectangle>&, std::function<void(int, int)>) const {
    return true;
}

bool PistolBullet::on_player_collision(int hit_player, const std::vector<Rectangle>&, std::function<void(int, int)> damage_player) const {
    damage_player(hit_player, 10);

    return true;
}

double PistolBullet::get_velocity() const {
    return 24;
}

const char* PistolBullet::bullet_image_name() const {
    return "bullet";
}