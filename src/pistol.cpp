#include "pistol.h"


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

void Pistol::render_large(RenderList& list) const {
    Rectangle rifle_box(0, 0, 50, 28);
    list.add_rect("pistol", rifle_box);
}

bool Pistol::has_explosive_bullets() const {
    return false;
}