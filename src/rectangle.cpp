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

bool Rectangle::intersects_line(double x_1, double y_1, double x_2, double y_2) const {
    // algorithm from http://stackoverflow.com/questions/99353/how-to-test-if-a-line-segment-intersects-an-axis-aligned-rectange-in-2d

    auto f = [&](double x, double y) {
        return  (y_2-y_1)*x + (x_1-x_2)*y + (x_2*y_1-x_1*y_2);
    };

    double values[4] = {
        f(x - width/2, y - height/2),
        f(x + width/2, y - height/2),
        f(x - width/2, y + height/2),
        f(x + width/2, y + height/2)
    };

    bool less = values[0] < 0;
    bool one_different = false;


    // printf("start (%f, %f) (%f, %f), (%f %f %f %f)\n", x_1, y_1, x_2, y_2, x, y, width, height);

    for (int i = 0; i < 4; i++) {
        bool current = values[i] < 0;
        // std::cout<<values[i]<<std::endl;

        if (less != current) {
            one_different = true;
        }
    }

    if (!one_different) {
        return false;
    }

    // std::cout<<"It's a possiblity"<<std::endl;

    if (x_1 > (x + width/2) && x_2 > (x + width/2)) return false;
    if (x_1 < (x - width/2) && x_2 < (x - width/2)) return false;

    if (y_1 > (y + height/2) && y_2 > (y + height/2)) return false;
    if (y_1 < (y - height/2) && y_2 < (y - height/2)) return false;

    // std::cout<<"Something broke"<<std::endl;

    return true;
}