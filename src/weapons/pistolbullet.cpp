#include "pistolbullet.h"

bool PistolBullet::on_wall_collision(const std::vector<Rectangle>&, std::function<void(int, double)>) {
    ticks_alive++;
    return true;
}

bool PistolBullet::on_player_collision(int hit_player, const std::vector<Rectangle>&, std::function<void(int, double)> damage_player) {
    ticks_alive++;
    damage_player(hit_player, 5);

    return true;
}

bool PistolBullet::on_no_collision() {
    ticks_alive++;
    return false;
}

double PistolBullet::get_velocity() const {
    return 24;
}

const char* PistolBullet::bullet_image_name() const {
    return "9mil";
}

void PistolBullet::render(RenderList& list) const {
    list.push();

    list.set_z(100);
    list.translate(pos.x, pos.y);
    list.rotate(angle);

    double max_dist = std::min(get_velocity() * ticks_alive, 100.0);

    list.add_scaled_image(bullet_image_name(), 0, 0, ASSET_SCALE, true);
    list.add_image("pistol-trail", -max_dist, -2, max_dist, 4);

    list.pop();
}