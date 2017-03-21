#include "gun.h"

#include "gameobjects/weapons/rocket/rocket.h"
#include "gameobjects/weapons/pistol/pistol.h"
#include "gameobjects/weapons/piercer/pierce.h"
#include "gameobjects/weapons/ricochet/bounce.h"
#include "gameobjects/weapons/flame/flame.h"

#include <cmath>
#include <iostream>

inline double dist_sq(double x1, double y1, double x2, double y2) {
    return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
}

Gun::~Gun() {}

void Gun::render_aim(RenderList& list, double gun_angle) const {
    list.push();
    list.translate(gun_rotation_x(), gun_rotation_y());

    list.rotate(gun_angle);

    list.add_scaled_image(gun_image_name(), gun_offset_x(), gun_offset_y(), ASSET_SCALE);//base_scale);
    list.pop();
}

void Gun::render(RenderList& list) const{
}

double Gun::grip1_x(double gun_angle) const {
    return gun_rotation_x() + cos(gun_angle) * grip1_dx() - sin(gun_angle) * grip1_dy();
}

double Gun::grip1_y(double gun_angle) const {
   return gun_rotation_y() + sin(gun_angle) * grip1_dx() + cos(gun_angle) * grip1_dy();
}

double Gun::grip2_x(double gun_angle) const {
    return gun_rotation_x() + cos(gun_angle) * grip2_dx() - sin(gun_angle) * grip2_dy();
}

double Gun::grip2_y(double gun_angle) const {
   return gun_rotation_y() + sin(gun_angle) * grip2_dx() + cos(gun_angle) * grip2_dy();
}

std::vector<std::unique_ptr<Bullet>> Gun::spawn_bullets(double gun_angle) const {

    std::vector<std::unique_ptr<Bullet>> results;

    double x = (gun_rotation_x() + cos(gun_angle) * (gun_offset_x() + barrel_offset_x() + barrel_length()) - sin(gun_angle) * (gun_offset_y() + barrel_offset_y()));
    double y = (gun_rotation_y() + sin(gun_angle) * (gun_offset_x() + barrel_offset_x() + barrel_length()) + cos(gun_angle) * (gun_offset_y() + barrel_offset_y()));

    std::unique_ptr<Bullet> result = create_initial_bullet();
    result->pos.x = x;
    result->pos.y = y;
    result->angle = 0;

    results.push_back(std::move(result));

    return results;
}

std::unique_ptr<Gun> create_gun(const std::string& gun_name) {
    if (gun_name == "rocket") {
        return std::make_unique<Rocket>();
    } else if (gun_name == "pistol") {
        return std::make_unique<Pistol>();
    } else if (gun_name == "pierce") {
        return std::make_unique<Pierce>();
    } else if (gun_name == "bounce") {
        return std::make_unique<Bounce>();
    } else if (gun_name == "flame") {
        return std::make_unique<Flame>();
    } else {
        std::cout<<"Cannot create gun of name " << gun_name << std::endl;
        exit(-1);
    }
}