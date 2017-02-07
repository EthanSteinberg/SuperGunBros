#include "level.h"

#include <fstream>

const double line_width = 4;

Level Level::load_from_file(const char* filename, unsigned int index) {
    std::ifstream file(filename);

    nlohmann::json level_data;

    file >> level_data;

    std::vector<Rectangle> obstacles;

    for (const auto& obstacle: level_data["obstacles"]) {
        double width = obstacle["xRight"].get<double>() - obstacle["xLeft"].get<double>();
        double height = obstacle["yBottom"].get<double>() - obstacle["yTop"].get<double>();
        Rectangle rect(
            obstacle["xRight"].get<double>() - width/2,
            obstacle["yBottom"].get<double>() - height/2,
            width,
            height);

        obstacles.push_back(rect);
    }


    std::vector<Rectangle> box_spawn_locations;
    for (const auto& spawn_location: level_data["boxSpawnLocations"]) {
        double width = spawn_location["xRight"].get<double>() - spawn_location["xLeft"].get<double>();
        double height = spawn_location["yBottom"].get<double>() - spawn_location["yTop"].get<double>();
        Rectangle rect(
            spawn_location["xRight"].get<double>() - width/2,
            spawn_location["yBottom"].get<double>() - height/2,
            width,
            height);

        box_spawn_locations.push_back(rect);
    }

    std::vector<Point> player_spawn_locations;
    for (const auto& spawn_location: level_data["spawnLocations"]) {
        Point p = {spawn_location["x"].get<double>(), spawn_location["y"].get<double>()};
        player_spawn_locations.push_back(p);
    }

    double width = level_data["width"].get<double>();
    double height = level_data["height"].get<double>();

    return Level(obstacles, box_spawn_locations, player_spawn_locations, width, height, index);
}

Level::Level(
    const std::vector<Rectangle>& a_obstacles,
    const std::vector<Rectangle>& a_box_spawns,
    const std::vector<Point>& a_player_spawns,
    double a_width,
    double a_height,
    unsigned int a_index):
    obstacles(a_obstacles), box_spawn_locations(a_box_spawns),
    player_spawn_locations(a_player_spawns),
    width(a_width),
    height(a_height),
    index(a_index) {}

void Level::render(RenderList& list, bool show_border) const {
    //TODO: Better background logic
    for(int i = 0; i < width; i+= 1280){
        for(int j = 0; j < height; j+= 720){
            list.add_image("background", i, j);
        }
    }

    if (show_border) {
        for (const Rectangle& rect : obstacles) {
            list.add_outline("black", rect, line_width);
        }

        for (const Rectangle& rect : obstacles) {
            list.add_image("darkGrey", rect.x - rect.width/2 + line_width, rect.y - rect.height/2 + line_width, rect.width - line_width * 2, rect.height - line_width * 2);
        }
    } else {
        for (const Rectangle& rect : obstacles) {
            list.add_image("darkGrey", rect.x - rect.width/2, rect.y - rect.height/2, rect.width, rect.height);
        }
    }
}

bool Level::colliding_with(const Rectangle& other) const {
    for (const Rectangle& rect : obstacles) {
        if (rect.colliding_with(other)) {
            return true;
        }
    }

    return false;
}

std::vector<Point> Level::get_player_spawn_locations() const {
    return player_spawn_locations;
}