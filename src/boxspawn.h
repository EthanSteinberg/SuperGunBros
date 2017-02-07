#ifndef BOX_SPAWN_H_INCLUDED
#define BOX_SPAWN_H_INCLUDED

#include "rectangle.h"
#include "weaponbox.h"
#include <random>

class BoxSpawn {
public:
    BoxSpawn(Rectangle spawn_location, std::vector<std::string> weapons_to_choose);

    template<typename Generator>
    WeaponBox get_random_spawn(Generator& g) const {
        std::uniform_real_distribution<> dx_dist(-spawn_location.width/2, spawn_location.width/2);
        std::uniform_real_distribution<> dy_dist(-spawn_location.height/2, spawn_location.height/2);

        std::uniform_int_distribution<> weapon_dist(0, weapons_to_choose.size() - 1);

        return WeaponBox(spawn_location.x + dx_dist(g), spawn_location.y + dy_dist(g), weapons_to_choose[weapon_dist(g)]);
    }

private:
    Rectangle spawn_location;
    std::vector<std::string> weapons_to_choose;
};

#endif


