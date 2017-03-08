#include "piercebullet.h"

#include <iostream>

bool PierceBullet::on_wall_collision(const std::vector<Rectangle>&, std::function<void(int, double)>) {
    switch (state) {
        case PierceBulletState::BEFORE_WALL:
            if (num_pierces_left == 0) {
                return true;
            }

            state = PierceBulletState::IN_WALL;
            return false;

        case PierceBulletState::IN_WALL:
            return false;

        default:
            std::cout<<"No such pierce state " << (int) state << std::endl;
            exit(-1);
    }
}

bool PierceBullet::on_player_collision(int hit_player, const std::vector<Rectangle>&, std::function<void(int, double)> damage_player) {
    damage_player(hit_player, 15);

    return true;
}

bool PierceBullet::on_no_collision() {
    switch (state) {
        case PierceBulletState::BEFORE_WALL:
            return false;

        case PierceBulletState::IN_WALL:
            state = PierceBulletState::BEFORE_WALL;
            num_pierces_left--;
            return false;

        default:
            std::cout<<"No such pierce state " << (int) state << std::endl;
            exit(-1);
    }
}

double PierceBullet::get_velocity() const {
    return 28;
}

const char* PierceBullet::bullet_image_name() const {
    return "piercing-round";
}