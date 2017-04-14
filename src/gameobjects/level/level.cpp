#include "level.h"

#include <fstream>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/error/en.h>

#include <iostream>

#include <exception>

const bool level_build_debug = false;

const char* const level_names[] = {
        // "../assets/level/testlevel.json",
        "../assets/level/platforms.json",
        "../assets/level/more_platforms.json",
        "../assets/level/shafts.json",
        "../assets/level/close_quarters.json",
        // "../assets/level/complex.json",
        "../assets/level/the_gap.json",
        "../assets/level/bnb.json"
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
        };

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

    std::vector<Obstacle> obstacles;

    for (const auto& obstacle: level_data["obstacles"].GetArray()) {
        double width = obstacle["xRight"].GetDouble() - obstacle["xLeft"].GetDouble();
        double height = obstacle["yBottom"].GetDouble() - obstacle["yTop"].GetDouble();

        double x = obstacle["xRight"].GetDouble() - width/2;
        double y = obstacle["yBottom"].GetDouble() - height/2;

        bool pierceable = !obstacle.HasMember("pierceable") || obstacle["pierceable"].GetBool();

        Rectangle rect(
            x,
            y,
            width,
            height);

        obstacles.push_back(Obstacle(rect, pierceable));

        bool is_mirrored = obstacle.HasMember("mirrored") ? obstacle["mirrored"].GetBool() : mirrored;

        if (is_mirrored) {
            Rectangle rect2(
                    l_width - x,
                    y,
                    width,
                    height);

            obstacles.push_back(Obstacle(rect2, pierceable));
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

    std::vector<Rectangle> killboxes;
    for (const auto& kb: level_data["killboxes"].GetArray()) {
        double width = kb["xRight"].GetDouble() - kb["xLeft"].GetDouble();
        double height = kb["yBottom"].GetDouble() - kb["yTop"].GetDouble();

        double x = kb["xRight"].GetDouble() - width/2;
        double y = kb["yBottom"].GetDouble() - height/2;

        Rectangle rect(
                x,
                y,
                width,
                height);

        killboxes.push_back(rect);

        bool is_mirrored = kb.HasMember("mirrored") ? kb["mirrored"].GetBool() : mirrored;

        if (is_mirrored) {
            Rectangle rect2(
                    l_width - x,
                    y,
                    width,
                    height);

            killboxes.push_back(rect2);
        }
    }

    std::string title = "untitled";
    if (level_data.HasMember("title")) {
        title = level_data["title"].GetString();
    }

    if (level_data.HasMember("background")){
        std::vector<std::shared_ptr<GameObject>> bgs;
        for(const auto& bg : level_data["background"].GetArray()) {

            std::string img = bg["img"].GetString();

            double width = bg["xRight"].GetDouble() - bg["xLeft"].GetDouble();
            double height = bg["yBottom"].GetDouble() - bg["yTop"].GetDouble();

            double x = bg["xRight"].GetDouble() - width / 2;
            double y = bg["yBottom"].GetDouble() - height / 2;

            Rectangle rect(
                    x,
                    y,
                    width,
                    height);

            bgs.push_back(std::make_shared<Background>(img, rect));
        }

        return Level(obstacles, box_spawn_locations, player_spawn_locations, killboxes, l_width, l_height, index, title, std::make_shared<CompoundBackground>(bgs));

    }


    return Level(obstacles, box_spawn_locations, player_spawn_locations, killboxes, l_width, l_height, index, title);
}

Level::Level(
    const std::vector<Obstacle>& a_obstacles,
    const std::vector<BoxSpawn>& a_box_spawns,
    const std::vector<Point>& a_player_spawns,
    const std::vector<Rectangle>& a_killboxes,
    double a_width,
    double a_height,
    unsigned int a_index,
    const std::string& a_title) :
        Level(
            a_obstacles,
            a_box_spawns,
            a_player_spawns,
            a_killboxes,
            a_width,
            a_height,
            a_index,
            a_title,
            std::make_shared<TileBackground>("tile", Rectangle(a_width/2, a_height/2, 2*a_width, 2*a_height))){}
            //TileBackground("tile", Rectangle(-a_width, -a_height, 2*a_width, 2*a_height))){}

Level::Level(
        const std::vector<Obstacle>& a_obstacles,
        const std::vector<BoxSpawn>& a_box_spawns,
        const std::vector<Point>& a_player_spawns,
        const std::vector<Rectangle>& a_killboxes,
        double a_width,
        double a_height,
        unsigned int a_index,
        const std::string& a_title,
        const std::shared_ptr<GameObject> bg):
        obstacles(a_obstacles),
        box_spawn_locations(a_box_spawns),
        player_spawn_locations(a_player_spawns),
        killboxes(a_killboxes),
        background(bg),
        width(a_width),
        height(a_height),
        index(a_index),
        title(a_title){}

void Level::render(RenderList& list) const {
    background->render(list);
    render_obstacles(list);

    if (level_build_debug) {

        list.add_outline("green", Rectangle(width/2, height/2, width, height), 3);

        for (auto &kb : killboxes) {
            list.add_outline("red", kb, 3);
        }

        for (auto &spawn : player_spawn_locations) {
            list.add_rect("blue", Rectangle(spawn.x, spawn.y, 10, 10));
        }

        for (auto &box : box_spawn_locations){
            list.add_rect("green", Rectangle(box.get_location().x, box.get_location().y, 10, 10));
        }
    }
}

void Level::render_thumbnail(RenderList &list) const {
    list.add_image("grey", 0, 0, width, height);
    render_obstacles(list, false);
}

void Level::render_obstacles(RenderList &list, bool show_border) const {
    if (show_border) {
        for (const Obstacle& obs : obstacles) {
            list.add_outline("black", obs.rect, line_width);
        }

        for (const Obstacle& obs : obstacles) {
            const char* color = obs.pierceable ? "darkGrey" : "nonPierce";
            list.add_image(color, obs.rect.x - obs.rect.width/2 + line_width, obs.rect.y - obs.rect.height/2 + line_width, obs.rect.width - line_width * 2, obs.rect.height - line_width * 2);
        }
    } else {
        for (const Obstacle& obs : obstacles) {
            const Rectangle& rect = obs.rect;
            double x1 = std::max(0.0, rect.x - rect.width/2);
            double x2 = std::min(width, rect.x + rect.width/2);


            double y1 = std::max(0.0, rect.y - rect.height/2);
            double y2 = std::min(height, rect.y + rect.height/2);

            const char* color = obs.pierceable ? "darkGrey" : "nonPierce";
            list.add_image(color, x1, y1, (x2 - x1), (y2 - y1));
        }
    }
}

bool Level::colliding_with(const Rectangle& other) const {
    for (const Obstacle& obs : obstacles) {
        if (obs.rect.colliding_with(other)) {
            return true;
        }
    }

    return false;
}

bool Level::intersects_line(double x_1, double y_1, double x_2, double y_2) const {
    for (const Obstacle& obs : obstacles) {
        if (obs.rect.intersects_line(x_1, y_1, x_2, y_2)) {
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

bool Level::in_killbox(double x, double y) const{
    for (auto& k : killboxes){
        if (k.contains_point(x, y)) return true;
    }
    return false;
}

double Level::get_first_non_pierce_intersection(double x, double y, double dx, double dy) const {
    double min_time = NAN;

    for (const Obstacle& obs: obstacles) {
        if (!obs.pierceable) {
            double time = obs.rect.get_ray_intersection(x, y, dx, dy);
            if (time == time) {
                if (min_time != min_time) {
                    min_time = time;
                } else {
                    min_time = std::min(min_time, time);
                }
            }
        }
    }

    if (min_time != min_time) {
        return std::max(width * 2, height * 2);
    }

    return min_time;
}

double Level::get_first_intersection(double x, double y, double dx, double dy) const {
    double min_time = NAN;

    for (const Obstacle& obs: obstacles) {
        double time = obs.rect.get_ray_intersection(x, y, dx, dy);
        if (time == time) {
            if (min_time != min_time) {
                min_time = time;
            } else {
                min_time = std::min(min_time, time);
            }
        }
    }

    if (min_time != min_time) {
        return std::max(width * 2, height * 2);
    }

    return min_time;
}

std::vector<std::pair<Point, Point>> Level::get_intersections(double x_1, double y_1, double x_2, double y_2) const {
    std::vector<std::pair<Point, Point>> results;

    for (const Obstacle& obs: obstacles) {
        std::pair<double, double> times = obs.rect.get_total_ray_intersection(x_1, y_1, x_2 - x_1, y_2 - y_1);
        if (times.first < 1) {
            double first_time = times.first;
            double second_time = std::min(1.0, times.second);
            results.push_back(std::make_pair(
                Point(x_1 + (x_2 - x_1) * first_time, y_1 + (y_2 - y_1) * first_time),
                Point(x_1 + (x_2 - x_1) * second_time, y_1 + (y_2 - y_1) * second_time)
                ));
        }
    }

    return results;
}