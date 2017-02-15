#include "weaponbox.h"
#include "magic.h"

WeaponBox::WeaponBox(float x, float y, std::string a_weapon) :
        pos(x, y, 86 * ASSET_SCALE, 86 *ASSET_SCALE), weapon(std::move(a_weapon)) {}

void WeaponBox::render(RenderList& list) const {
    if (!opened) {
        list.add_scaled_image("box", pos.x, pos.y, ASSET_SCALE, true);
    } else {
        Rectangle weapon_rectangle = list.get_image_dimensions(weapon);
        list.add_rect(weapon, weapon_rectangle.offset(pos.x, pos.y));
    }
}