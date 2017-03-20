#include "bounce.h"
#include "bouncebullet.h"

double Bounce::gun_rotation_x() const {
    return 0;
}

double Bounce::gun_rotation_y() const {
    return -20;
}

double Bounce::gun_offset_x() const {
    return 4;
}

double Bounce::gun_offset_y() const {
    return -10;
}

double Bounce::barrel_offset_x() const {
    return 10;
}

double Bounce::barrel_offset_y() const {
    return 3;
}

const char* Bounce::gun_image_name() const {
    return "ricochet-rifle";
}

bool Bounce::in_front() const {
    return true;
}

double Bounce::grip1_dx() const {
    return gun_offset_x() + 15;
}

double Bounce::grip1_dy() const {
    return gun_offset_y() + 10;
}

double Bounce::grip2_dx() const {
    return gun_offset_x() + 21;
}

double Bounce::grip2_dy() const {
    return gun_offset_y() + 8;
}

std::unique_ptr<Bullet> Bounce::create_initial_bullet() const {
    return std::make_unique<BounceBullet>();
}