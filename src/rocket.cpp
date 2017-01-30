#include "rocket.h"

double Rocket::gun_rotation_x() const {
    return 0;
}

double Rocket::gun_rotation_y() const {
    return -20;
}

double Rocket::gun_offset_x() const {
    return -10;
}

double Rocket::gun_offset_y() const {
    return -8;
}

double Rocket::barrel_offset_x() const {
    return 0;
}

double Rocket::barrel_offset_y() const {
    return 7;
}

const char* Rocket::gun_image_name() const {
    return "rocket";
}

bool Rocket::in_front() const {
    return false;
}

double Rocket::grip1_dx() const {
    return gun_offset_x() + 25;
}

double Rocket::grip1_dy() const {
    return gun_offset_y() + 13;
}

double Rocket::grip2_dx() const {
    return gun_offset_x() + 20;
}

double Rocket::grip2_dy() const {
    return gun_offset_y() + 13;
}

void Rocket::render_large(RenderList& list) const {
    Rectangle rocket(0, 0, 51, 19);
    list.add_rect("rocket", rocket);
}

bool Rocket::has_explosive_bullets() const {
    return true;
}