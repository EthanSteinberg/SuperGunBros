//
// Created by Chris Brown on 1/13/17.
//

#ifndef SUPERGUNBROS_PHYSICSENGINE_H
#define SUPERGUNBROS_PHYSICSENGINE_H

#include <map>
#import "gameobjects/level/level.h"
#import "gameobjects/player/player.h"
#import "gameobjects/weapons/explosion.h"
#import "soundthread.h"

#define DRAG_COEF 0.075
#define JUMP_STR 15
#define GRAVITY 0.75
#define X_ACCEL 0.5
#define JUMP_DUR 10
#define SIGMA 0.001
#define BOOST_STR 1
#define BOOST_DUR 180
#define WALL_FRICTION 0.2
#define WALL_GRACE 5
#define GROUND_GRACE 5

const int FIRE_LENGTH = 200;

const double FIRE_TRANSMIT_PROBABILITY = 0.1;

struct State{
    Level& level;
    std::vector<Player>& players;
    std::vector<std::unique_ptr<Bullet>>& bullets;
    std::vector<WeaponBox>& boxes;
    std::vector<Explosion>& explosions;
    //std::vector<GameObject> misc;
};


class PhysicsEngine {
    void update(State s, const std::map<int, inputs>& all_joystick_inputs, const std::map<int, inputs>& all_last_inputs, SoundThread sounds);
private:
    void update_players(State s, const std::map<int, inputs>& all_joystick_inputs, const std::map<int, inputs>& all_last_inputs);
    void update_bullets(State s);
    void update_explosions(State s);
    void damage_player(State s, int player_index, double damage, int shooter_index);
    std::mt19937 gen;

};


#endif //SUPERGUNBROS_PHYSICSENGINE_H
