#include "bullet.h"

static uint64_t next_id = 0;

Bullet::Bullet() : pos(0, 0, 6, 6) {
    id = next_id++;
}

Bullet::~Bullet() {}

void Bullet::render(RenderList& list) const {
    list.push();

    list.translate(pos.x, pos.y);
    list.rotate(angle);

    list.add_scaled_image(bullet_image_name(), 0, 0, ASSET_SCALE, true);

    //Rectangle image = list.get_image_dimensions(bullet_image_name());
    //list.add_rect(bullet_image_name(), image);

    list.pop();
}

bool Bullet::create_explosion_after_destruction() const {
    return false;
}

bool Bullet::catch_on_fire() const {
    return false;
}