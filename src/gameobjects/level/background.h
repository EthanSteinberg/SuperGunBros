//
// Created by Chris Brown on 3/26/17.
//

#ifndef SUPERGUNBROS_BACKGROUND_H_H
#define SUPERGUNBROS_BACKGROUND_H_H

#include "gameobjects/gameobject.h"
#include <memory>

class Background : public GameObject {
protected:
    std::string img;
    Rectangle r;
public:
    Background(const std::string image, Rectangle rect);
    virtual ~Background() {}

    virtual void render(RenderList& list) const;
    virtual void update();
};

class CompoundBackground : public GameObject {
private:
   std::vector<std::shared_ptr<GameObject>> bgs;
public:
   CompoundBackground(std::vector<std::shared_ptr<GameObject>> backgrounds);
   void render(RenderList& list) const override;
   void update() override;
};

class TileBackground : public Background {
public:
    TileBackground(std::string image, Rectangle rect);
    void render(RenderList& list) const override;
};

#endif //SUPERGUNBROS_BACKGROUND_H_H
