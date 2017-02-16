#include "weaponbox.h"
#include "magic.h"

#include <iostream>

WeaponBox::WeaponBox(float x, float y, std::string a_weapon, int a_ticks_until_active, int a_spawn_index) :
        opened(false),
        pos(x, y, 86 * ASSET_SCALE, 86 *ASSET_SCALE),
        weapon(std::move(a_weapon)),
        ticks_until_active(a_ticks_until_active),
        spawn_index(a_spawn_index) {}

void WeaponBox::render(RenderList& list) const {
    if (ticks_until_active != 0) {
        // Box is not active yet.
        return;
    }

    if (!opened) {
        list.add_scaled_image("box", pos.x, pos.y, ASSET_SCALE, true);
    } else {
        Rectangle weapon_rectangle = list.get_image_dimensions(weapon);
        list.add_rect(weapon, weapon_rectangle.offset(pos.x, pos.y));
    }
}