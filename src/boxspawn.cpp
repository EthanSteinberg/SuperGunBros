#include "boxspawn.h"

BoxSpawn::BoxSpawn(Rectangle a_spawn_location, std::vector<std::string> a_weapons_to_choose) :
    spawn_location(a_spawn_location), weapons_to_choose(std::move(a_weapons_to_choose)) {}