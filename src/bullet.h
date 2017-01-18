#ifndef BULLET_H_INCLUDED
#define BULLET_H_INCLUDED

const double BULLET_VEL = 0.3;

struct Bullet {
    unsigned int player_owner;

    double x;
    double y;
    double x_vel;
    double y_vel;
};

#endif