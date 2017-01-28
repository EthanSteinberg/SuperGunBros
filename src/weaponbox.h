#ifndef WEAPON_BOX_H_INCLUDED
#define WEAPON_BOX_H_INCLUDED

#include "rectangle.h"
#include "renderlist.h"

class WeaponBox {
public:
    Rectangle pos;

    bool opened;

    void render(RenderList& list) const;
};

#endif