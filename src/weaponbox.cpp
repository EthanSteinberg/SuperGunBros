#include "weaponbox.h"

void WeaponBox::render(RenderList& list) const {
    if (!opened) {
        list.add_rect("red", pos);
    } else {
        list.add_image("rocket", pos.x - 51.0/2, pos.y - 19.0/2);
    }
}