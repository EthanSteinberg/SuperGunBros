#ifndef BOX_SPAWN_H_INCLUDED
#define BOX_SPAWN_H_INCLUDED

#include "rectangle.h"
#include "gameobjects/weapons/weaponbox.h"
#include <random>
#include "gameobjects/weapons/gun.h"

class BoxSpawn {
public:
    BoxSpawn(Point spawn_location, std::vector<std::string> weapons_to_choose, int ticks_until_first_spawn, int ticks_respawn, int index);

    template<typename Generator>
    WeaponBox get_random_spawn(Generator& g, bool first_spawn) const {
        std::uniform_int_distribution<> weapon_dist(0, weapons_to_choose.size() - 1);

        int ticks = first_spawn ? ticks_until_first_spawn : ticks_respawn;

        return WeaponBox(spawn_location.x, spawn_location.y, create_gun(weapons_to_choose[weapon_dist(g)]), ticks, index);
    }

    Point get_location() const;

private:

    Point spawn_location;
    std::vector<std::string> weapons_to_choose;

public:
    int ticks_until_first_spawn;
    int ticks_respawn;
    int index;
};

#endif


