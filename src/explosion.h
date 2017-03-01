#ifndef EXPLOSION_H_INCLUDED
#define EXPLOSION_H_INCLUDED

#include "renderlist.h"

const int EXPLOSION_RADIUS = 50;

class Explosion {
public:
    Explosion(double x, double y, bool little_explosion = false);
    void update();

    void render(RenderList& list) const;

    bool is_done() const;

private:
    double time = 0.8;
    double x;
    double y;

    bool little_explosion;
};

#endif