#ifndef WEAPON_BOX_H_INCLUDED
#define WEAPON_BOX_H_INCLUDED

#include "rectangle.h"
#include "rendering/renderlist.h"

#include <memory>
#include "gameobjects/weapons/gun.h"

class WeaponBox : public GameObject {
public:

    WeaponBox(float x, float y, std::unique_ptr<Gun> weapon, int ticks_until_active, int spawn_index);

    bool opened;

    Rectangle pos;

    void render(RenderList& list) const;
    void update(){};

    std::unique_ptr<Gun> weapon;

    int ticks_until_active;
    int spawn_index;
};

#endif