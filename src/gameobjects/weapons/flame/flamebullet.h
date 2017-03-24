#ifndef FLAME_BULLET_H_INCLUDED
#define FLAME_BULLET_H_INCLUDED

#include "gameobjects/weapons/bullet.h"

const double MAX_LIFE = 30.0;

class FlameBullet : public Bullet {
public:

    FlameBullet(bool individual = false);

    virtual bool on_wall_collision(const std::vector<Rectangle>& player_positions, std::function<void(int, double)> damage_player) override;
    virtual bool on_player_collision(int hit_player, const std::vector<Rectangle>& player_positions, std::function<void(int, double)> damage_player) override;
    virtual bool on_no_collision() override;

    virtual double get_velocity() const override;

    virtual void render(RenderList& list) const override;

    virtual bool catch_on_fire() const override;

    virtual bool can_block() override {
        return true;
    }

// private:

    int ticks_left = MAX_LIFE;
    int ticks_alive = 0;
private:

    bool individual;

    bool count_down_life();

    virtual const char* bullet_image_name() const override;
};

#endif