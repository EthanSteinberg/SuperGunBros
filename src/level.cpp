//
// Created by Chris Brown on 1/13/17.
//

#include "level.h"

Level::Level() {
    width = 0;
    height = 0;
}

Level::Level(int w, int h, std::vector<std::vector<Terrain>> g) {
    width = w;
    height = h;
    grid = g;
}

Terrain Level::get_terrain(int x, int y) {
    if(0 <= x < width && 0 <= y < height){
        return grid[x][y];
    } else {
        fprintf(stdout, "oops\n");
        return (Terrain) -1; //TODO exception?
    }
}

bool Level::is_obstacle(int x, int y) {
    return get_terrain(x, y) != Terrain ::OPEN;
}

void Level::draw(RenderList& r) {
// Render the map
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            if (get_terrain(x, y) == Terrain ::GROUND) {
                r.add_image("black", x * 60, y * 60, 60, 60); //TODO there should probably be a renderer class
            }
        }
    }
}