#ifndef BOUNCE_BULLET_H_INCLUDED
#define BOUNCE_BULLET_H_INCLUDED

#include "gameobjects/weapons/bullet.h"

const int BOUNCE_BULLET_LIFE = 180;

class BounceBullet : public Bullet {
public:

    virtual bool on_wall_collision(const std::vector<Rectangle>& player_positions, std::function<void(int, double)> damage_player, std::function<void(int, double, double)> push_back_player, bool free_horizontal, bool free_vertical, SoundThread& sounds) override;
    virtual bool on_player_collision(int hit_player, const std::vector<Rectangle>& player_positions, std::function<void(int, double)> damage_player) override;
    virtual bool on_no_collision() override;

    virtual double get_velocity() const override;

    virtual bool can_damage_self() const override {
        return true;
    }


    virtual ExplosionType get_explosion() const override {
        return ExplosionType::BOUNCE;
    }


private:

    int ticks_left = BOUNCE_BULLET_LIFE;

    virtual const char* bullet_image_name() const override;
};

#endif