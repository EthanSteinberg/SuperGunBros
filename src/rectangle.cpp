#include "rectangle.h"

#include <cmath>

Rectangle::Rectangle(double a_x, double a_y, double a_width, double a_height) : x(a_x), y(a_y), width(a_width), height(a_height) {}


bool Rectangle::colliding_with(const Rectangle& other) const {
    double total_width = width + other.width;
    double total_height = height + other.height;

    return (fabs(x - other.x) < total_width / 2 && fabs(y - other.y) < total_height / 2);
}