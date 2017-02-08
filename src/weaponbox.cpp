#include "weaponbox.h"

WeaponBox::WeaponBox(float x, float y, std::string a_weapon) : pos(x, y, 40, 40), weapon(std::move(a_weapon)) {}

void WeaponBox::render(RenderList& list) const {
    if (!opened) {
        list.add_rect("red", pos);
    } else {
        Rectangle weapon_rectangle = list.get_image_dimensions(weapon);
        list.add_rect(weapon, weapon_rectangle.offset(pos.x, pos.y));
    }
}