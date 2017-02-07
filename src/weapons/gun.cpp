#include "gun.h"

#include "rocket.h"
#include "pistol.h"
#include "pierce.h"

//TODO: make a constants file for this, this base scale is going to be everywhere
float base_scale = 0.4;

inline double dist_sq(double x1, double y1, double x2, double y2) {
    return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
}

Gun::~Gun() {}

void Gun::render(RenderList& list, double gun_angle) const {
    list.translate(gun_rotation_x(), gun_rotation_y());

    list.rotate(gun_angle);

    list.add_scaled_image(gun_image_name(), gun_offset_x(), gun_offset_y(), 1);//base_scale);

    list.rotate(-gun_angle);

    list.translate(-gun_rotation_x(), -gun_rotation_y());
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

std::unique_ptr<Bullet> Gun::spawn_bullet(double gun_angle) const {

    double x = (gun_rotation_x() + cos(gun_angle) * (gun_offset_x() + barrel_offset_x() + 20) - sin(gun_angle) * (gun_offset_y() + barrel_offset_y()));
    double y = (gun_rotation_y() + sin(gun_angle) * (gun_offset_x() + barrel_offset_x() + 20) + cos(gun_angle) * (gun_offset_y() + barrel_offset_y()));

    std::unique_ptr<Bullet> result = create_initial_bullet();
    result->pos.x = x;
    result->pos.y = y;

    return result;
}

std::unique_ptr<Gun> create_gun(const std::string& gun_name) {
    if (gun_name == "rocket") {
        return std::make_unique<Rocket>();
    } else if (gun_name == "pistol") {
        return std::make_unique<Pistol>();
    } else if (gun_name == "pierce") {
        return std::make_unique<Pierce>();
    } else {
        std::cout<<"Cannot create gun of name " << gun_name << std::endl;
        exit(-1);
    }
}

void Gun::render_large(RenderList& list) const {
    Rectangle image = list.get_image_dimensions(gun_image_name());
    list.add_rect(gun_image_name(), image);
}