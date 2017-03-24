#ifndef EXPLOSION_H_INCLUDED
#define EXPLOSION_H_INCLUDED

#include "rendering/renderlist.h"

const int EXPLOSION_RADIUS = 50;

enum class ExplosionType {
    ROCKET,
    BOUNCE,
    PISTOL,
    NONE
};

class Explosion {
public:
    Explosion(double x, double y, ExplosionType type);
    void update();

    void render(RenderList& list) const;

    bool is_done() const;

private:
    double time = 0.8;
    double x;
    double y;

    ExplosionType type;
};

#endif