#ifndef LEVEL_H_INCLUDED
#define LEVEL_H_INCLUDED

#include "rectangle.h"
#include <vector>
#include "renderlist.h"
#include "point.h"
#include "weaponbox.h"
#include "boxspawn.h"

#include <random>

class Level {
public:
    static Level load_from_file(const char* filename, unsigned int index);

    void render(RenderList& list, bool show_border = true) const;

    bool colliding_with(const Rectangle& other) const;

    template<typename Generator>
    WeaponBox get_random_box_spawn(Generator& g) const {
        std::uniform_int_distribution<> box_dis(0, box_spawn_locations.size() - 1);

        return box_spawn_locations[box_dis(g)].get_random_spawn(g);
    }

    std::vector<Point> get_player_spawn_locations() const;

    template<typename Generator>
    Point get_random_player_spawn_location(Generator& g) const {
        std::uniform_int_distribution<> box_dis(0, player_spawn_locations.size() - 1);

        return player_spawn_locations[box_dis(g)];
    }

private:
    Level(
        const std::vector<Rectangle>& obstacles,
        const std::vector<BoxSpawn>& box_spawn_locations,
        const std::vector<Point>& player_spawn_locations,
        double width,
        double height,
        unsigned int index);

    const std::vector<Rectangle> obstacles;

    const std::vector<BoxSpawn> box_spawn_locations;

    const std::vector<Point> player_spawn_locations;

public:
    const double width;
    const double height;
    const unsigned int index;
};

#endif