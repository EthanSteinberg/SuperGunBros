#ifndef RECTANGLE_H_INCLUDED
#define RECTANGLE_H_INCLUDED

#include "point.h"
#include <utility>

class Rectangle {
public:
    /**
     * The x and y here is the center of the rectangle.
     */
    Rectangle(double x, double y, double width, double height);

    bool colliding_with(const Rectangle& other) const;

    bool contains_point(double p_x, double p_y) const;

    bool intersects_line(double x_1, double y_1, double x_2, double y_2) const;

    std::pair<double, double> get_total_ray_intersection(double x, double y, double dx, double dy) const;
    double get_ray_intersection(double x, double y, double dx, double dy) const;

    Rectangle offset(double dx, double dy) const;

    Point location() const;

    double x;
    double y;
    double width;
    double height;
};

#endif