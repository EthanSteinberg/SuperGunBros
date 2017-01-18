//
// Created by Chris Brown on 1/13/17.
//

#ifndef SUPERGUNBROS_LEVEL_H
#define SUPERGUNBROS_LEVEL_H

#include "renderlist.h"

enum class Terrain {
    OPEN,
    GROUND
};


class Level {
    int width;
    int height;
    std::vector<std::vector<Terrain>> grid;
public:
    Level();
    Level(int w, int h, std::vector<std::vector<Terrain>> g);
    bool is_obstacle(int x, int y);
    Terrain get_terrain(int x, int y);
    void draw(RenderList& r);
};


#endif //SUPERGUNBROS_LEVEL_H
