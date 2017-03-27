//
// Created by Chris Brown on 3/26/17.
//

#ifndef SUPERGUNBROS_BACKGROUND_H_H
#define SUPERGUNBROS_BACKGROUND_H_H

#include "gameobjects/gameobject.h"

class Background : public GameObject{
protected:
    std::string img;
    Rectangle r;
public:
    Background(const std::string image, Rectangle rect);
    virtual void render(RenderList& list) const;
    virtual void update();
};
//
//class CompoundBackground : public Background{
//private:
//    std::vector<Background> bgs;
//public:
//    CompoundBackground(std::vector<Background>& backgrounds);
//    void render(RenderList& list) const override;
//    void update() override;
//};

class TileBackground : public Background{
public:
    TileBackground(std::string image, Rectangle rect);
    void render(RenderList& list) const override;
};

#endif //SUPERGUNBROS_BACKGROUND_H_H
