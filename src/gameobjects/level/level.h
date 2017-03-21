#ifndef LEVEL_H_INCLUDED
#define LEVEL_H_INCLUDED

#include "rectangle.h"
#include <vector>
#include "rendering/renderlist.h"
#include "point.h"
#include "gameobjects/weapons/weaponbox.h"
#include "boxspawn.h"

#include <random>

class Level : public GameObject {
public:
    static std::vector<Level> load_all_levels();

    void render(RenderList& list) const;

    void render_thumbnail(RenderList& list) const;

    void update() {}

    bool colliding_with(const Rectangle& other) const;

    const std::vector<BoxSpawn>& get_box_spawns() const;

    std::vector<Point> get_player_spawn_locations() const;

    template<typename Generator>
    Point get_random_player_spawn_location(Generator& g) const {
        std::uniform_int_distribution<> box_dis(0, player_spawn_locations.size() - 1);

        return player_spawn_locations[box_dis(g)];
    }

private:
    static Level load_from_file(const char* filename, unsigned int index);
    void render_obstacles(RenderList& list, bool show_border=true) const;
    void render_background(RenderList& list) const;

    Level(
        const std::vector<Rectangle>& obstacles,
        const std::vector<BoxSpawn>& box_spawn_locations,
        const std::vector<Point>& player_spawn_locations,
        double width,
        double height,
        unsigned int index,
        const std::string& title,
        const std::string& background = "tile"
    );

    std::vector<Rectangle> obstacles;

    std::vector<BoxSpawn> box_spawn_locations;

    std::vector<Point> player_spawn_locations;

    std::string background_img;

public:
    double width;
    double height;
    unsigned int index;
    std::string title;
};

#endif