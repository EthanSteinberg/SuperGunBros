#ifndef POINT_H_INCLUDED
#define POINT_H_INCLUDED

struct Point {
    double x;
    double y;

    double dist(const Point& other);
};

#endif