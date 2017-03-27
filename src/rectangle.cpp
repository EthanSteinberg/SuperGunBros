#include "rectangle.h"

#include <cmath>
#include <iostream>

Rectangle::Rectangle(double a_x, double a_y, double a_width, double a_height) : x(a_x), y(a_y), width(a_width), height(a_height) {}


bool Rectangle::colliding_with(const Rectangle& other) const {
    double total_width = width + other.width;
    double total_height = height + other.height;

    return Rectangle(x, y, total_width, total_height).contains_point(other.x, other.y);
}

bool Rectangle::contains_point(double p_x, double p_y) const {
    return (fabs(x - p_x) < width / 2 && fabs(y - p_y) < height / 2);
}

Rectangle Rectangle::offset(double dx, double dy) const {
    return Rectangle(x + dx, y + dy, width, height);
}

Point Rectangle::location() const {
    return Point{x, y};
}

double intersect_ray(Point A, Point Aslope, Point B, Point Bslope) {

    double determinant = -Aslope.x * Bslope.y + Aslope.y * Bslope.x;

    if (determinant == 0) {
        return NAN;
    }

    // Now compute an intersection point using a matrix inverse

    double t1 = (-Bslope.y * (B.x - A.x) + Bslope.x * (B.y - A.y)) / determinant;
    double t2 = (-Aslope.y * (B.x - A.x) + Aslope.x * (B.y - A.y)) / determinant;

    if (t1 < 0 || t1 > 1) {
        return NAN;
    }

    if (t2 < 0) {
        return NAN;
    }

    return t2;
}

double Rectangle::get_ray_intersection(double p_x, double p_y, double dx, double dy) const {
    double intercepts[4] = {};
    Point b(p_x, p_y);
    Point b_slope(dx, dy);

    intercepts[0] = intersect_ray(Point(x - width/2, y - height/2), Point( width,       0), b, b_slope);
    intercepts[1] = intersect_ray(Point(x - width/2, y + height/2), Point(     0, -height), b, b_slope);
    intercepts[2] = intersect_ray(Point(x + width/2, y - height/2), Point(     0,  height), b, b_slope);
    intercepts[3] = intersect_ray(Point(x + width/2, y + height/2), Point(-width,       0), b, b_slope);


    double min_intercept = NAN;

    for (int i = 0; i < 4; i++) {
        if (intercepts[i] == intercepts[i]) {
            if (min_intercept != min_intercept) {
                min_intercept = intercepts[i];
            } else {
                min_intercept = std::min(min_intercept, intercepts[i]);
            }
        }
    }

    return min_intercept;
}

bool Rectangle::intersects_line(double x_1, double y_1, double x_2, double y_2) const {
    double t = get_ray_intersection(x_1, y_1, x_2 - x_1, y_2 - y_1);

    if (t != t) {
        return false;
    }

    if (t > 1) {
        return false;
    }

    return true;
}