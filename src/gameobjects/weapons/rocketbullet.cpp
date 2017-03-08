#include "rocketbullet.h"

#include "gameobjects/explosion.h"

void RocketBullet::perform_explosion(const std::vector<Rectangle>& player_positions, std::function<void(int, double)> damage_player) const {
    for (unsigned int i = 0; i < player_positions.size(); i++) {
        auto& player_pos = player_positions[i];

        // TODO: Actual circle, rect intersection test

        double dx = abs(player_pos.x - pos.x);
        double dy = abs(player_pos.y - pos.y);

        bool hit =  (dx <= player_pos.width / 2 && dy <= (player_pos.height /2 + EXPLOSION_RADIUS)) ||
                    (dx <= (player_pos.width / 2 + EXPLOSION_RADIUS) && dy <= player_pos.height / 2 ) ||
                    (((dx - player_pos.width) * (dx - player_pos.width)  +
                        (dy - player_pos.height) * (dy - player_pos.height)) <= EXPLOSION_RADIUS * EXPLOSION_RADIUS);

        if (hit) {
            damage_player(i, 15);
        }
    }
}

bool RocketBullet::on_wall_collision(const std::vector<Rectangle>& player_positions, std::function<void(int, double)> damage_player) {
    perform_explosion(player_positions, damage_player);
    return true;
}

bool RocketBullet::on_player_collision(int hit_player, const std::vector<Rectangle>& player_positions, std::function<void(int, double)> damage_player) {
    perform_explosion(player_positions, damage_player);
    damage_player(hit_player, 10);
    return true;
}


bool RocketBullet::on_no_collision() {
    return false;
}


double RocketBullet::get_velocity() const {
    return 16;
}

const char* RocketBullet::bullet_image_name() const {
    return "missile";
}

bool RocketBullet::create_explosion_after_destruction() const {
    return true;
}