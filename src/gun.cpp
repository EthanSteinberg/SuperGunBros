#include "gun.h"

inline double dist_sq(double x1, double y1, double x2, double y2) {
    return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
}

inline double calculate_angle(double target_x, double target_y, double center_x, double center_y, double barrel_x, double barrel_y) {

    double theta = atan2(center_y - barrel_y, barrel_x - center_x);

    double barrel_offset_sq = dist_sq(barrel_x, barrel_y, center_x, center_y);
    double target_offset_sq = dist_sq(target_x, target_y, center_x, center_y);

    double far_angle = asin(sqrt(barrel_offset_sq) * sin(theta) / sqrt(target_offset_sq));

    double starting_angle = atan2(target_y - center_y, target_x - center_x);

    return starting_angle + far_angle;
}


double Gun::aim_at(double mouseX, double mouseY) const {
    double first_angle_assuming_right = calculate_angle(
        mouseX, mouseY,
        gun_rotation_x(),
        gun_rotation_y(),
        gun_rotation_x() + gun_offset_x() + barrel_offset_x(),
        gun_rotation_y() + gun_offset_y() + barrel_offset_y()
    );

    if (fabs(first_angle_assuming_right) < M_PI / 2) {
        return first_angle_assuming_right;
    } else {
        return calculate_angle(
                    mouseX, mouseY,
                    -gun_rotation_x(),
                    gun_rotation_y(),
                    -(gun_rotation_x() + gun_offset_x() + barrel_offset_x()),
                    gun_rotation_y() + gun_offset_y() + barrel_offset_y()
                );
    }

}

void Gun::render(RenderList& list, double gun_angle) const {
    list.translate(gun_rotation_x(), gun_rotation_y());

    list.rotate(gun_angle);

    list.add_image(gun_image_name(), gun_offset_x(), gun_offset_y());

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

Bullet Gun::spawn_bullet(double gun_angle) const {

    double x = (gun_rotation_x() + cos(gun_angle) * (gun_offset_x() + barrel_offset_x() + 20) - sin(gun_angle) * (gun_offset_y() + barrel_offset_y()));
    double y = (gun_rotation_y() + sin(gun_angle) * (gun_offset_x() + barrel_offset_x() + 20) + cos(gun_angle) * (gun_offset_y() + barrel_offset_y()));

    Bullet result(x, y, gun_angle);

    result.is_explosive_bullet = has_explosive_bullets();

    return result;
}