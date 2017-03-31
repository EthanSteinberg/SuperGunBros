#ifndef POINT_H_INCLUDED
#define POINT_H_INCLUDED

struct Point {
    Point(double a_x, double a_y): x(a_x), y(a_y) {}

    double x;
    double y;

    double dist(const Point& other);

    Point offset(Point other) const {
        return Point(x + other.x, y + other.y);
    }
};

#endif