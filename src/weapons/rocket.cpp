#include "rocket.h"
#include "rocketbullet.h"

double Rocket::gun_rotation_x() const {
    return 0;
}

double Rocket::gun_rotation_y() const {
    return -20;
}

double Rocket::gun_offset_x() const {
    return -30;
}

double Rocket::gun_offset_y() const {
    return -15;
}

double Rocket::barrel_offset_x() const {
    return 0;
}

double Rocket::barrel_offset_y() const {
    return 7;
}

const char* Rocket::gun_image_name() const {
    return "rocket_launcher";
}

bool Rocket::in_front() const {
    return false;
}

double Rocket::grip1_dx() const {
    return gun_offset_x() + 50;
}

double Rocket::grip1_dy() const {
    return gun_offset_y() + 15;
}

double Rocket::grip2_dx() const {
    return gun_offset_x() + 40;
}

double Rocket::grip2_dy() const {
    return gun_offset_y() + 10;
}

std::unique_ptr<Bullet> Rocket::create_initial_bullet() const {
    return std::make_unique<RocketBullet>();
}