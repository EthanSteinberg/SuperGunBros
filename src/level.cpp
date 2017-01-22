#include "level.h"

#include <fstream>

const double line_width = 4;

Level Level::load_from_file(const char* filename) {
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

    return Level(obstacles);
}

Level::Level(const std::vector<Rectangle>& a_obstacles): obstacles(a_obstacles) {}


void Level::render(RenderList& list, bool show_border) const {

    if (show_border) {
        for (const Rectangle& rect : obstacles) {
            list.add_line("black", rect.x - rect.width/2, rect.y - rect.height/2 + line_width / 2, rect.x + rect.width/2, rect.y - rect.height/2 + line_width/2);
            list.add_line("black", rect.x - rect.width/2, rect.y + rect.height/2 - line_width / 2, rect.x + rect.width/2, rect.y + rect.height/2 - line_width/2);
            list.add_line("black", rect.x - rect.width/2 + line_width / 2, rect.y - rect.height/2, rect.x - rect.width/2 + line_width / 2, rect.y + rect.height/2);
            list.add_line("black", rect.x + rect.width/2 - line_width / 2, rect.y - rect.height/2, rect.x + rect.width/2 - line_width / 2, rect.y + rect.height/2);
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
