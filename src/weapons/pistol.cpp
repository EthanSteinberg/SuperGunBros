#include "pistol.h"
#include "pistolbullet.h"

double Pistol::gun_rotation_x() const {
    return 1;
}

double Pistol::gun_rotation_y() const {
    return -20;
}

double Pistol::gun_offset_x() const {
    return 3;
}

double Pistol::gun_offset_y() const {
    return 2;
}

double Pistol::barrel_offset_x() const {
    return 0;
}

double Pistol::barrel_offset_y() const {
    return 3;
}

const char* Pistol::gun_image_name() const {
    return "pistol";
}

bool Pistol::in_front() const {
    return true;
}

double Pistol::grip1_dx() const {
    return gun_offset_x() + 2;
}

double Pistol::grip1_dy() const {
    return gun_offset_y() + 7;
}

double Pistol::grip2_dx() const {
    return gun_offset_x() + 10;
}

double Pistol::grip2_dy() const {
    return gun_offset_y() + 6;
}

std::unique_ptr<Bullet> Pistol::create_initial_bullet() const {
    return std::make_unique<PistolBullet>();
}