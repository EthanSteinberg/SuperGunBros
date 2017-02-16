#ifndef WEAPON_BOX_H_INCLUDED
#define WEAPON_BOX_H_INCLUDED

#include "rectangle.h"
#include "renderlist.h"

class WeaponBox {
public:

    WeaponBox(float x, float y, std::string weapon, int ticks_until_active, int spawn_index);

    bool opened;

    Rectangle pos;

    void render(RenderList& list) const;

    std::string weapon;

    int ticks_until_active;
    int spawn_index;
};

#endif