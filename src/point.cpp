#include "point.h"
#include <cmath>

double Point::dist(const Point& other) {
    return std::sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y));
}