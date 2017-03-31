#ifndef LEVEL_H_INCLUDED
#define LEVEL_H_INCLUDED

#include "rectangle.h"
#include <vector>
#include "rendering/renderlist.h"
#include "point.h"
#include "gameobjects/weapons/weaponbox.h"
#include "boxspawn.h"
#include "background.h"
#include "obstacle.h"

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

    bool intersects_line(double x_1, double y_1, double x_2, double y_2) const;

    bool in_killbox(double x, double y) const;

    double get_first_non_pierce_intersection(double x, double y, double dx, double dy) const;

    std::vector<std::pair<Point, Point>> get_intersections(double x_1, double y_1, double x_2, double y_2) const;

    template<typename Generator>
    Point get_random_player_spawn_location(Generator& g) const {
        std::uniform_int_distribution<> box_dis(0, player_spawn_locations.size() - 1);
        return player_spawn_locations[box_dis(g)];
    }

private:
    static Level load_from_file(const char* filename, unsigned int index);
    void render_obstacles(RenderList& list, bool show_border=true) const;

    Level(
            const std::vector<Obstacle>& obstacles,
            const std::vector<BoxSpawn>& box_spawn_locations,
            const std::vector<Point>& player_spawn_locations,
            const std::vector<Rectangle>& killboxes,
            double width,
            double height,
            unsigned int index,
            const std::string& title
    );

    Level(
            const std::vector<Obstacle>& obstacles,
            const std::vector<BoxSpawn>& box_spawn_locations,
            const std::vector<Point>& player_spawn_locations,
            const std::vector<Rectangle>& killboxes,
            double width,
            double height,
            unsigned int index,
            const std::string& title,
            const std::shared_ptr<Background> background
    );

    std::vector<Obstacle> obstacles;
    std::vector<BoxSpawn> box_spawn_locations;
    std::vector<Point> player_spawn_locations;
    std::vector<Rectangle> killboxes;
    std::shared_ptr<Background> background;

public:
    double width;
    double height;
    unsigned int index;
    std::string title;
};

#endif