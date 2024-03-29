#include "bullet.h"

static uint64_t next_id = 0;

Bullet::Bullet() : pos(0, 0, 6, 6) {
    id = next_id++;
}

Bullet::~Bullet() {}

void Bullet::render(RenderList& list) const {
    list.push();

    list.set_z(100);
    list.translate(pos.x, pos.y);
    list.rotate(angle);

    list.add_scaled_image(bullet_image_name(), 0, 0, ASSET_SCALE, true);

    list.pop();
}

bool Bullet::catch_on_fire() const {
    return false;
}