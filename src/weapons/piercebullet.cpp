#include "piercebullet.h"

bool PierceBullet::on_wall_collision(const std::vector<Rectangle>&, std::function<void(int, int)>) const {
    return false;
}

bool PierceBullet::on_player_collision(int hit_player, const std::vector<Rectangle>&, std::function<void(int, int)> damage_player) const {
    damage_player(hit_player, 20);

    return true;
}

double PierceBullet::get_velocity() const {
    return 12;
}

const char* PierceBullet::bullet_image_name() const {
    return "pierce-bullet";
}