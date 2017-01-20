#ifndef BULLET_H_INCLUDED
#define BULLET_H_INCLUDED

#include "rectangle.h"

const double BULLET_VEL = 4;

struct Bullet {
    Bullet(double x, double y, double angle) : pos(x, y, 6, 6), x_vel(BULLET_VEL * cos(angle)), y_vel(BULLET_VEL * sin(angle)) {}

    unsigned int player_owner;

    Rectangle pos;

    double x_vel;
    double y_vel;
};

#endif