#include "flame.h"
#include "flamebullet.h"

#include <cmath>

#include <iostream>

double Flame::gun_rotation_x() const {
    return 0;
}

double Flame::gun_rotation_y() const {
    return -20;
}

double Flame::gun_offset_x() const {
    return -10;
}

double Flame::gun_offset_y() const {
    return -8;
}

double Flame::barrel_offset_x() const {
    return 0;
}

double Flame::barrel_offset_y() const {
    return 7;
}

const char* Flame::gun_image_name() const {
    return "flame";
}

bool Flame::in_front() const {
    return false;
}

double Flame::grip1_dx() const {
    return gun_offset_x() + 25;
}

double Flame::grip1_dy() const {
    return gun_offset_y() + 13;
}

double Flame::grip2_dx() const {
    return gun_offset_x() + 20;
}

double Flame::grip2_dy() const {
    return gun_offset_y() + 13;
}

std::unique_ptr<Bullet> Flame::create_initial_bullet() const {
    return std::make_unique<FlameBullet>();
}

double Flame::barrel_length() const {
    return 40;
}

std::vector<std::unique_ptr<Bullet>> Flame::spawn_bullets(double gun_angle) const {
    std::vector<std::unique_ptr<Bullet>> results;

    double x = (gun_rotation_x() + cos(gun_angle) * (gun_offset_x() + barrel_offset_x() + barrel_length()) - sin(gun_angle) * (gun_offset_y() + barrel_offset_y()));
    double y = (gun_rotation_y() + sin(gun_angle) * (gun_offset_x() + barrel_offset_x() + barrel_length()) + cos(gun_angle) * (gun_offset_y() + barrel_offset_y()));

    for (int i = -10; i <= 10; i++) {
        std::unique_ptr<Bullet> result = create_initial_bullet();
        result->pos.x = x;
        result->pos.y = y;
        result->angle = i/10.0 * 15.0 * M_PI/180.0;

        FlameBullet* foo = (FlameBullet*) result.get();

        std::cout<<"Start " << foo->ticks_left<<std::endl;

        results.push_back(std::move(result));
    }

    return results;
}