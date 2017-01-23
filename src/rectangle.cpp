#include "rectangle.h"

#include <cmath>

Rectangle::Rectangle(double a_x, double a_y, double a_width, double a_height) : x(a_x), y(a_y), width(a_width), height(a_height) {}


bool Rectangle::colliding_with(const Rectangle& other) const {
    double total_width = width + other.width;
    double total_height = height + other.height;

    return Rectangle(x, y, total_width, total_height).contains_point(other.x, other.y);
}

bool Rectangle::contains_point(double p_x, double p_y) const {
    return (fabs(x - p_x) < width / 2 && fabs(y - p_y) < height / 2);
}