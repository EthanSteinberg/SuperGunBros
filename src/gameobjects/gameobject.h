//
// Created by Chris Brown on 3/8/17.
//

#ifndef SUPERGUNBROS_GAMEOBJECT_H
#define SUPERGUNBROS_GAMEOBJECT_H

#include "rendering/renderlist.h"


class GameObject {
    virtual void render(RenderList& list) const = 0;
    virtual void update() = 0;
    //virtual bool collides_with(GameObject other) const = 0;
    //virtual void interact_with(GameObject other) const = 0;
    //virtual bool can_reap() const = 0;
};


#endif //SUPERGUNBROS_GAMEOBJECT_H
