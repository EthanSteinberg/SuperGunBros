#include "bounce.h"
#include "bouncebullet.h"

double Bounce::gun_rotation_x() const {
    return 0;
}

double Bounce::gun_rotation_y() const {
    return -20;
}

double Bounce::gun_offset_x() const {
    return -10;
}

double Bounce::gun_offset_y() const {
    return -8;
}

double Bounce::barrel_offset_x() const {
    return 0;
}

double Bounce::barrel_offset_y() const {
    return 7;
}

const char* Bounce::gun_image_name() const {
    return "bounce";
}

bool Bounce::in_front() const {
    return false;
}

double Bounce::grip1_dx() const {
    return gun_offset_x() + 25;
}

double Bounce::grip1_dy() const {
    return gun_offset_y() + 13;
}

double Bounce::grip2_dx() const {
    return gun_offset_x() + 20;
}

double Bounce::grip2_dy() const {
    return gun_offset_y() + 13;
}

std::unique_ptr<Bullet> Bounce::create_initial_bullet() const {
    return std::make_unique<BounceBullet>();
}