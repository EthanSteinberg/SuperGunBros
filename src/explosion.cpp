#include "explosion.h"

const double LITTLE_EXPLOSION_RADIUS = 10;

Explosion::Explosion(double a_x, double a_y, bool a_little_explosion) : x(a_x), y(a_y), little_explosion(a_little_explosion) {}


void Explosion::update() {
    time -= 0.04;
}

bool Explosion::is_done() const {
    return time <= 0.0;
}

void Explosion::render(RenderList& list) const {

    double whole_radius = little_explosion ? LITTLE_EXPLOSION_RADIUS : EXPLOSION_RADIUS;
    double radius = whole_radius * (1 - time * time);

    Rectangle explosion_rect(x, y, radius * 2, radius * 2);
    list.add_rect(little_explosion ? "bounce-explosion" : "ballOfFire", explosion_rect);
}