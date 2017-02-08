#ifndef WEAPON_BOX_H_INCLUDED
#define WEAPON_BOX_H_INCLUDED

#include "rectangle.h"
#include "renderlist.h"

class WeaponBox {
public:

    WeaponBox(float x, float y, std::string weapon);

    bool opened;

    Rectangle pos;

    void render(RenderList& list) const;

    std::string weapon;
};

#endif