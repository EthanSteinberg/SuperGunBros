#include "explosion.h"

#include <iostream>

double get_radius(ExplosionType type) {
    switch (type) {
        case ExplosionType::ROCKET:
            return EXPLOSION_RADIUS;

        case ExplosionType::BOUNCE:
            return 10;

        case ExplosionType::PISTOL:
            return 6;

        case ExplosionType ::NONE:
            break;
    }

    std::cout<<"Invalid explosion type " << (int) type << std::endl;
    abort();
}

const char* get_image(ExplosionType type) {
    switch (type) {
        case ExplosionType::ROCKET:
            return "ballOfFire";

        case ExplosionType::BOUNCE:
            return "bounce-explosion";

        case ExplosionType::PISTOL:
            return "pistol-explosion";

        case ExplosionType ::NONE:
            break;
    }

    std::cout<<"Invalid explosion type " << (int) type << std::endl;
    abort();
}

double get_decay_rate(ExplosionType type) {
    switch (type) {
        case ExplosionType::ROCKET:
            return 0.04;

        case ExplosionType::BOUNCE:
            return 0.08;

        case ExplosionType::PISTOL:
            return 0.08;
    }

    std::cout<<"Invalid explosion type " << (int) type << std::endl;
    abort();
}

Explosion::Explosion(double a_x, double a_y, ExplosionType a_type) : x(a_x), y(a_y), type(a_type) {

    if (type == ExplosionType::NONE) {
        std::cout<<"Trying to create a non-existant explosion?"<<std::endl;
        abort();
    }
}


void Explosion::update() {
    time -= get_decay_rate(type);
}

bool Explosion::is_done() const {
    return time <= 0.0;
}

void Explosion::render(RenderList& list) const {
    list.push();
    list.set_z(100);

    double radius = get_radius(type) * (1 - time * time);

    Rectangle explosion_rect(x, y, radius * 2, radius * 2);
    list.add_rect(get_image(type), explosion_rect);
    list.pop();
}