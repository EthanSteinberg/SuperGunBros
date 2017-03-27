//
// Created by Chris Brown on 3/26/17.
//

#include <gameobjects/gameobject.h>
#include "background.h"

Background::Background(const std::string image, Rectangle rect) : img(image), r(rect) {}

void Background::render(RenderList& list) const {
    list.add_rect(img, r);
}

void Background::update(){}

//CompoundBackground::CompoundBackground(std::vector<Background>& backgrounds) : bgs(backgrounds){}
//
//void CompoundBackground::render(RenderList& list) const{
//    for (auto& bg : bgs) {
//        bg.render(list);
//    }
//}
//
//void CompoundBackground::update(){
//    for (auto& bg : bgs) {
//            bg.update();
//    }
//}

TileBackground::TileBackground(std::string image, Rectangle rect) : Background(image, rect){}

void TileBackground::render(RenderList& list) const{
    Rectangle bg_dim = list.get_image_dimensions(img);
    double width = r.width;
    double height = r.height;
    Point loc = r.location();
    for (int i = (int)(loc.x - width/2); i < loc.x + width/2; i += bg_dim.width) {
        for (int j = (int)(loc.y - height/2); j < loc.y + height/2; j += bg_dim.height) {
            list.add_image(img, i, j);
        }
    }
}
//TODO: animated backgrounds (the only reason we dispatch all of the updates in compound background)
