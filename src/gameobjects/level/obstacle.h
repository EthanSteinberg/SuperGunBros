#ifndef OBSTACLE_H_INCLUDED
#define OBSTACLE_H_INCLUDED

#include "rectangle.h"

class Obstacle {
public:
    Obstacle(Rectangle a_rect, bool a_pierceable): rect(a_rect), pierceable(a_pierceable) {}

    Rectangle rect;
    bool pierceable;
};

#endif