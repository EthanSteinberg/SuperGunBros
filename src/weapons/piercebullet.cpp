#include "piercebullet.h"

#include <iostream>

bool PierceBullet::on_wall_collision(const std::vector<Rectangle>&, std::function<void(int, double)>) {
    switch (state) {
        case PierceBulletState::INIT:
            state = PierceBulletState::HIT_WALL;
            return false;

        case PierceBulletState::HIT_WALL:
            return false;

        case PierceBulletState::AFTER_HIT_WALL:
            return true;

        default:
            std::cout<<"No such pierce state " << (int) state << std::endl;
            exit(-1);
    }
}

bool PierceBullet::on_player_collision(int hit_player, const std::vector<Rectangle>&, std::function<void(int, double)> damage_player) {
    damage_player(hit_player, 20);

    return true;
}

bool PierceBullet::on_no_collision() {
    switch (state) {
        case PierceBulletState::INIT:
            return false;

        case PierceBulletState::HIT_WALL:
            state = PierceBulletState::AFTER_HIT_WALL;
            return false;

        case PierceBulletState::AFTER_HIT_WALL:
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
    return "pierce-bullet";
}