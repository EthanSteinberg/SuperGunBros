//
// Created by Chris Brown on 1/13/17.
//

#ifndef SUPERGUNBROS_PHYSICSENGINE_H
#define SUPERGUNBROS_PHYSICSENGINE_H

#include <map>
#import "gameobjects/level/level.h"
#import "gameobjects/player.h"
#import "gameobjects/explosion.h"

struct State{
    Level level;
    std::vector<Player> players;
    std::vector<std::unique_ptr<Bullet>> bullets;
    std::vector<WeaponBox> boxes;
    std::vector<Explosion> explosions;
    //std::vector<Effects> effects;
};


class PhysicsEngine {
    void update(State s, const std::map<int, inputs>& all_joystick_inputs, const std::map<int, inputs>& all_last_inputs);
private:
    //std::vector<std::unique_ptr<Bullet>> update_bullets();
    std::mt19937 gen;

};


#endif //SUPERGUNBROS_PHYSICSENGINE_H
