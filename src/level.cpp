#include "level.h"

#include <fstream>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/error/en.h>

#include <iostream>

#include <exception>

const char* const level_names[] = {
    "../assets/level/level_1.json",
    "../assets/level/level_2.json",

    "../assets/level/level_4.json",

    "../assets/level/size_1_template.json",
    "../assets/level/size_2_template.json",
    "../assets/level/size_3_template.json"
};

const double line_width = 4;

std::vector<Level> Level::load_all_levels() {
    std::vector<Level> loaded_levels;
    for (const auto& level: level_names) {
        int i = loaded_levels.size();
        loaded_levels.push_back(Level::load_from_file(level, i));
    }

    return loaded_levels;
}

Level Level::load_from_file(const char* filename, unsigned int index) {
    std::ifstream file(filename);

    rapidjson::BasicIStreamWrapper<std::ifstream> wrapper(file);

    rapidjson::Document level_data;

    level_data.ParseStream<rapidjson::kParseCommentsFlag | rapidjson::kParseTrailingCommasFlag>(wrapper);

    if (level_data.HasParseError()) {
        throw std::runtime_error(std::string("Could not parse ") + filename);
    }

    double l_width = level_data["width"].GetDouble();
    double l_height = level_data["height"].GetDouble();

    bool mirrored = false;

    if (level_data.HasMember("mirrored")){
        mirrored = level_data["mirrored"].GetBool();
    }

    std::vector<Rectangle> obstacles;

    for (const auto& obstacle: level_data["obstacles"].GetArray()) {
        double width = obstacle["xRight"].GetDouble() - obstacle["xLeft"].GetDouble();
        double height = obstacle["yBottom"].GetDouble() - obstacle["yTop"].GetDouble();

        double x = obstacle["xRight"].GetDouble() - width/2;
        double y = obstacle["yBottom"].GetDouble() - height/2;

        Rectangle rect(
            x,
            y,
            width,
            height);

        obstacles.push_back(rect);

        if (mirrored) {
            Rectangle rect2(
                    l_width - x,
                    y,
                    width,
                    height);

            obstacles.push_back(rect2);
        }
    }


    std::vector<BoxSpawn> box_spawn_locations;
    for (const auto& spawn_location: level_data["boxSpawnLocations"].GetArray()) {
        double width = spawn_location["xRight"].GetDouble() - spawn_location["xLeft"].GetDouble();
        double height = spawn_location["yBottom"].GetDouble() - spawn_location["yTop"].GetDouble();

        double x = spawn_location["xRight"].GetDouble() - width/2;
        double y = spawn_location["yBottom"].GetDouble() - height/2;

        Rectangle rect(
            x,
            y,
            width,
            height);

        std::vector<std::string> weapons;

        for (const auto& weapon : spawn_location["weapons"].GetArray()) {
            weapons.push_back(weapon.GetString());
        }

        box_spawn_locations.push_back(BoxSpawn(rect, weapons));

        if (mirrored) {
            Rectangle rect2(
                    l_width - x,
                    y,
                    width,
                    height);

            box_spawn_locations.push_back(BoxSpawn(rect2,weapons));
        }

    }

    std::vector<Point> player_spawn_locations;
    for (const auto& spawn_location: level_data["spawnLocations"].GetArray()) {

        double x = spawn_location["x"].GetDouble();
        double y = spawn_location["y"].GetDouble();
        Point p = {x, y};
        player_spawn_locations.push_back(p);

        if (mirrored) {
            Point p2 = {l_width-x, y};
            player_spawn_locations.push_back(p2);
        }
    }

    return Level(obstacles, box_spawn_locations, player_spawn_locations, l_width, l_height, index);
}

Level::Level(
    const std::vector<Rectangle>& a_obstacles,
    const std::vector<BoxSpawn>& a_box_spawns,
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

            if (show_border) {
                list.add_image("background", i, j);
            } else {
                list.add_image("grey", i, j, 1280, 720);
            }

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