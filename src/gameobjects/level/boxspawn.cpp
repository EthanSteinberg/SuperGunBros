#include "boxspawn.h"

Point BoxSpawn::get_location() const {
    return spawn_location;
}

BoxSpawn::BoxSpawn(Point a_spawn_location, std::vector<std::string> a_weapons_to_choose, int a_ticks_until_first_spawn, int a_ticks_respawn, int a_index) :
    spawn_location(a_spawn_location),
    weapons_to_choose(std::move(a_weapons_to_choose)),
    ticks_until_first_spawn(a_ticks_until_first_spawn),
    ticks_respawn(a_ticks_respawn),
    index(a_index) {}