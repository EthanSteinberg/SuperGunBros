#include "rocketbullet.h"

#include "gameobjects/weapons/explosion.h"
#include <cmath>
#include <iostream>

void RocketBullet::perform_explosion(const std::vector<Rectangle>& player_positions, std::function<void(int, double)> damage_player, std::function<void(int, double, double)> push_back_player) const {
    for (unsigned int i = 0; i < player_positions.size(); i++) {
        auto& player_pos = player_positions[i];

        // TODO: Actual circle, rect intersection test

        double dx = std::abs(player_pos.x - pos.x);
        double dy = std::abs(player_pos.y - pos.y);

        bool hit =  (dx <= player_pos.width / 2 && dy <= (player_pos.height /2 + EXPLOSION_RADIUS)) ||
                    (dx <= (player_pos.width / 2 + EXPLOSION_RADIUS) && dy <= player_pos.height / 2 ) ||
                    (((dx - player_pos.width) * (dx - player_pos.width)  +
                        (dy - player_pos.height) * (dy - player_pos.height)) <= EXPLOSION_RADIUS * EXPLOSION_RADIUS);

        if (hit) {
            double offset_dx1 = player_pos.x - pos.x;
            double offset_dy1 = (player_pos.y - 60 * ASSET_SCALE) - pos.y;

            double distance1 = sqrt(offset_dx1 * offset_dx1 + offset_dy1 * offset_dy1);

            double offset_dx2 = player_pos.x - pos.x;
            double offset_dy2 = (player_pos.y + 20 * ASSET_SCALE) - pos.y;

            double distance2 = sqrt(offset_dx2 * offset_dx2 + offset_dy2 * offset_dy2);

            double f_dx;
            double f_dy;

            if (distance1 < distance2) {
                f_dx = offset_dx1/distance1;
                f_dy = offset_dy1/distance1;
            } else {
                f_dx = offset_dx2/distance2;
                f_dy = offset_dy2/distance2;
            }

            double force_factor = 25;
            push_back_player(i, force_factor * f_dx, force_factor * f_dy);

            damage_player(i, 15);
        }
    }
}

bool RocketBullet::on_wall_collision(const std::vector<Rectangle>& player_positions, std::function<void(int, double)> damage_player, std::function<void(int, double, double)> push_back_player) {
    ticks_alive++;
    perform_explosion(player_positions, damage_player, push_back_player);
    return true;
}

bool RocketBullet::on_player_collision(int hit_player, const std::vector<Rectangle>& player_positions, std::function<void(int, double)> damage_player, std::function<void(int, double, double)> push_back_player) {
    ticks_alive++;
    perform_explosion(player_positions, damage_player, push_back_player);
    damage_player(hit_player, 10);
    return true;
}

bool RocketBullet::on_blocker_collision(const std::vector<Rectangle>& player_positions, std::function<void(int, double)> damage_player, std::function<void(int, double, double)> push_back_player) {
    ticks_alive++;
    perform_explosion(player_positions, damage_player, push_back_player);
    return true;
}

bool RocketBullet::on_no_collision() {
    ticks_alive++;
    return false;
}


double RocketBullet::get_velocity() const {
    return 16;
}

const char* RocketBullet::bullet_image_name() const {
    return "missile";
}

void RocketBullet::render(RenderList& list) const {
    list.push();

    list.set_z(100);
    list.translate(pos.x, pos.y);
    list.rotate(angle);

    double max_dist = std::min(get_velocity() * ticks_alive, 100.0);

    list.add_scaled_image(bullet_image_name(), 0, 0, ASSET_SCALE, true);
    list.add_image("rocket-trail", -25 - max_dist, -2, max_dist, 4);

    list.pop();
}