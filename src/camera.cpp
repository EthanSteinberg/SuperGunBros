#include "camera.h"

#include <iostream>

const double SCREEN_WIDTH = 1280.0;
const double SCREEN_HEIGHT = 720.0;

Camera::Camera(double a_level_width, double a_level_hight)
    : x(-1), y(-1), scale(1), level_width(a_level_width), level_height(a_level_hight) {}

void Camera::transform(RenderList& list) const {
    double width = SCREEN_WIDTH * scale;
    double height = SCREEN_HEIGHT * scale;

    list.scale(1.0/scale, 1.0/scale);
    list.translate(-(x - width/2), -(y - height/2));
}

void Camera::update(const std::vector<Point>& player_positions) {
    if (player_positions.size() == 0) {
        // There are no players;
        return;
    }

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
    maxX = std::min(level_width, maxX + 200);

    minY = minY - 100  - 100.0 * scale;
    maxY = std::min(level_height, maxY + 200);

    double desired_x = (minX + maxX)/2.0;
    double desired_y = (minY + maxY)/2.0;

    double widthRatio = (maxX - minX)/SCREEN_WIDTH;
    double heightRatio = (maxY - minY)/SCREEN_HEIGHT;

    double desired_scale = std::max(widthRatio, heightRatio);

    if (x == -1 || y == -1) {
        // First update, do not interpolate
        x = desired_x;
        y = desired_y;
        scale = desired_scale;
    } else {
        scale = 0.95 * scale + 0.05 * desired_scale;
        x = 0.95 * x + 0.05 * desired_x;
        y = 0.95 * y + 0.05 * desired_y;

        y = std::min(level_height -  SCREEN_HEIGHT * scale * 0.5, y);
    }
}