#include "level.h"

#include <fstream>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/error/en.h>

#include <iostream>

#include <exception>

const char* const level_names[] = {
        "../assets/level/platforms.json",
        "../assets/level/more_platforms.json",
        "../assets/level/shafts.json",
        "../assets/level/close_quarters.json",
};

const double line_width = 4;

std::vector<Level> Level::load_all_levels() {
    std::vector<Level> loaded_levels;
    for (const auto& level: level_names) {
        int i = loaded_levels.size();
        try {
            Level loaded_level = Level::load_from_file(level, i);
            loaded_levels.push_back(std::move(loaded_level));
        } catch (const std::exception& ex) {
            std::cout<<"Got error while reloading " << level << ":" << ex.what() <<std::endl;
        }

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

        bool is_mirrored = obstacle.HasMember("mirrored") ? obstacle["mirrored"].GetBool() : mirrored;

        if (is_mirrored) {
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

        double x = spawn_location["x"].GetDouble();
        double y = spawn_location["y"].GetDouble();

        Point pos{
            x,
            y};

        std::vector<std::string> weapons;

        for (const auto& weapon : spawn_location["weapons"].GetArray()) {
            weapons.push_back(weapon.GetString());
        }

        int initial_spawn_delay = 100;
        int respawn_delay = 100;

        if (spawn_location.HasMember("initialSpawnDelay")) {
            initial_spawn_delay = spawn_location["initialSpawnDelay"].GetInt();
        }

        if (spawn_location.HasMember("respawnDelay")) {
            respawn_delay = spawn_location["respawnDelay"].GetInt();
        }

        box_spawn_locations.push_back(BoxSpawn(pos, weapons, initial_spawn_delay, respawn_delay, box_spawn_locations.size()));

        bool is_mirrored = spawn_location.HasMember("mirrored") ? spawn_location["mirrored"].GetBool() : mirrored;

        if (is_mirrored)  {
            Point pos2{
                    l_width - x,
                    y};

            box_spawn_locations.push_back(BoxSpawn(pos2,weapons, initial_spawn_delay, respawn_delay, box_spawn_locations.size()));
        }

    }

    std::vector<Point> player_spawn_locations;
    for (const auto& spawn_location: level_data["spawnLocations"].GetArray()) {

        double x = spawn_location["x"].GetDouble();
        double y = spawn_location["y"].GetDouble();
        Point p = {x, y};
        player_spawn_locations.push_back(p);

        bool is_mirrored = spawn_location.HasMember("mirrored") ? spawn_location["mirrored"].GetBool() : mirrored;

        if (is_mirrored)  {
            Point p2 = {l_width-x, y};
            player_spawn_locations.push_back(p2);
        }
    }

    std::string title = "untitled";
    if (level_data.HasMember("title")) {
        title = level_data["title"].GetString();
    }

    return Level(obstacles, box_spawn_locations, player_spawn_locations, l_width, l_height, index, title);
}

Level::Level(
    const std::vector<Rectangle>& a_obstacles,
    const std::vector<BoxSpawn>& a_box_spawns,
    const std::vector<Point>& a_player_spawns,
    double a_width,
    double a_height,
    unsigned int a_index,
    const std::string& a_title):
    obstacles(a_obstacles), box_spawn_locations(a_box_spawns),
    player_spawn_locations(a_player_spawns),
    width(a_width),
    height(a_height),
    index(a_index),
    title(a_title) {}

void Level::render(RenderList& list, bool show_border) const {
    //TODO: Better background logic
    for(int i = 0; i < width; i+= 1280){
        for(int j = 0; j < height; j+= 720){

            if (show_border) {
                list.add_image("background", i, j);
            }

        }
    }

    if (!show_border) {
        list.add_image("grey", 0, 0, width, height);
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
            double x1 = std::max(0.0, rect.x - rect.width/2);
            double x2 = std::min(width, rect.x + rect.width/2);


            double y1 = std::max(0.0, rect.y - rect.height/2);
            double y2 = std::min(height, rect.y + rect.height/2);

            list.add_image("darkGrey", x1, y1, (x2 - x1), (y2 - y1));
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

const std::vector<BoxSpawn>& Level::get_box_spawns() const {
    return box_spawn_locations;
}