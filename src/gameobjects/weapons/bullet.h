#ifndef BULLET_H_INCLUDED
#define BULLET_H_INCLUDED

#include "rectangle.h"
#include <vector>
#include <functional>
#include "rendering/renderlist.h"
#include "gameobjects/weapons/explosion.h"
#include <cstdint>

class SoundThread;

class Bullet {
public:
    Bullet();

    virtual ~Bullet();

    unsigned int player_owner;

    Rectangle pos;

    double angle;

    uint64_t id;

    virtual void render(RenderList& list) const;

    // Return true if the bullet should be marked dead
    virtual bool on_wall_collision(const std::vector<Rectangle>& player_positions, std::function<void(int, double)> damage_player, bool free_horizontal, bool free_vertical, SoundThread& sounds) {
        return on_wall_collision(player_positions, damage_player);
    }

    virtual bool on_player_collision(int hit_player, const std::vector<Rectangle>& player_positions, std::function<void(int, double)> damage_player) = 0;
    virtual bool on_no_collision() = 0;

    virtual bool on_blocker_collision(const std::vector<Rectangle>& player_positions, std::function<void(int, double)> damage_player) {
        return true;
    }

    virtual bool can_block() {
        return false;
    }

    virtual double get_velocity() const = 0;

    virtual ExplosionType get_explosion() const{
        return ExplosionType::NONE;
    }

    virtual bool catch_on_fire() const;

    virtual bool pierce_special_effect() const {
        return false;
    }

    virtual bool can_damage_self() const {
        return false;
    }

private:
    virtual const char* bullet_image_name() const = 0;

    virtual bool on_wall_collision(const std::vector<Rectangle>& player_positions, std::function<void(int, double)> damage_player) {return false;}

};

#endif