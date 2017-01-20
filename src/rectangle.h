#ifndef RECTANGLE_H_INCLUDED
#define RECTANGLE_H_INCLUDED

class Rectangle {
public:
    Rectangle(double x, double y, double width, double height);

    bool colliding_with(const Rectangle& other) const;

    double x;
    double y;
    double width;
    double height;
};

#endif