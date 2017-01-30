#include "explosion.h"

Explosion::Explosion(double a_x, double a_y) : x(a_x), y(a_y) {}


void Explosion::update() {
    time -= 0.04;
}

bool Explosion::is_done() const {
    return time <= 0.0;
}

void Explosion::render(RenderList& list) const {
    Rectangle explosion_rect(x, y, EXPLOSION_RADIUS * 2 * (1 - time * time), EXPLOSION_RADIUS * 2 * (1 - time * time));
    list.add_rect("ballOfFire", explosion_rect);
}