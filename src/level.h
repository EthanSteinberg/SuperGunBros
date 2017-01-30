#ifndef LEVEL_H_INCLUDED
#define LEVEL_H_INCLUDED

#include "rectangle.h"
#include <vector>
#include "renderlist.h"
#include "point.h"

#include <random>

class Level {
public:
    static Level load_from_file(const char* filename);

    void render(RenderList& list, bool show_border = true) const;

    bool colliding_with(const Rectangle& other) const;

    template<typename Generator>
    Point get_random_box_spawn_location(Generator& g) const {
        std::uniform_int_distribution<> box_dis(0, box_spawn_locations.size() - 1);

        Rectangle location = box_spawn_locations[box_dis(g)];

        std::uniform_real_distribution<> dx_dis(-location.width/2, location.width/2);
        std::uniform_real_distribution<> dy_dis(-location.height/2, location.height/2);

        return {location.x + dx_dis(g), location.y + dy_dis(g)};
    }

private:
    Level(const std::vector<Rectangle>& obstacles, const std::vector<Rectangle>& box_spawn_locations);

    const std::vector<Rectangle> obstacles;

    const std::vector<Rectangle> box_spawn_locations;
};

#endif