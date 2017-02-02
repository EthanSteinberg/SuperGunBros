#include "camera.h"

Camera::Camera() : x(1280/2), y(720/2), scale(1) {}

void Camera::transform(RenderList& list) const {
    double width = 1280 * scale;
    double height = 720 * scale;

    list.scale(1.0/scale, 1.0/scale);
    list.translate(-(x - width/2), -(y - height/2));
}

void Camera::update(const std::vector<Point>& player_positions) {
    double minX = player_positions[0].x;
    double maxX = player_positions[0].x;

    double minY = player_positions[0].y;
    double maxY = player_positions[0].y;

    for (const Point& pos: player_positions) {
        minX = std::min(pos.x, minX);
        maxX = std::max(pos.x, maxX);

        minY = std::min(pos.y, minY);
        maxY = std::max(pos.y, maxY);
    }

    minX = std::max(0.0, minX - 200);
    maxX = std::min(1280.0, maxX + 200);

    minY = std::max(0.0, minY - 200);
    maxY = std::min(720.0, maxY + 200);

    double desired_x = (minX + maxX)/2.0;
    double desired_y = (minY + maxY)/2.0;

    double widthRatio = (maxX - minX)/1280.0;
    double heightRatio = (maxY - minY)/720.0;

    double desired_scale = std::max(widthRatio, heightRatio);


    scale = 0.95 * scale + 0.05 * desired_scale;
    x = 0.95 * x + 0.05 * desired_x;
    y = 0.95 * y + 0.05 * desired_y;

    x = std::max(1280 * scale * 0.5, x);
    x = std::min(1280 - 1280 * scale * 0.5, x);

    y = std::max(720 * scale * 0.5, y);
    y = std::min(720 -  720 * scale * 0.5, y);
}