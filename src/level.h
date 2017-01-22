#ifndef LEVEL_H_INCLUDED
#define LEVEL_H_INCLUDED

#include "rectangle.h"
#include <vector>
#include "renderlist.h"

class Level {
public:
    static Level load_from_file(const char* filename);

    void render(RenderList& list, bool show_border = true) const;

    bool colliding_with(const Rectangle& other) const;

private:
    Level(const std::vector<Rectangle>& obstacles);

    const std::vector<Rectangle> obstacles;
};

#endif