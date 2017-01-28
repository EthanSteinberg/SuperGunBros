#ifndef RECTANGLE_H_INCLUDED
#define RECTANGLE_H_INCLUDED

#include "point.h"

class Rectangle {
public:
    /**
     * The x and y here is the center of the rectangle.
     */
    Rectangle(double x, double y, double width, double height);

    bool colliding_with(const Rectangle& other) const;

    bool contains_point(double p_x, double p_y) const;

    Rectangle offset(double dx, double dy) const;

    Point location() const;

    double x;
    double y;
    double width;
    double height;
};

#endif