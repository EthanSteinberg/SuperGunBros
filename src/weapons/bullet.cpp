#include "bullet.h"
#include "magic.h"

Bullet::Bullet() : pos(0, 0, 6, 6) {}

Bullet::~Bullet() {}

void Bullet::render(RenderList& list) const {
    list.push();

    list.translate(pos.x, pos.y);
    list.rotate(angle);

    list.add_scaled_image(bullet_image_name(), 0, 0, ASSET_SCALE);

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