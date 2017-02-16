#ifndef FLAME_BULLET_H_INCLUDED
#define FLAME_BULLET_H_INCLUDED

#include "bullet.h"

class FlameBullet : public Bullet {
public:

    virtual bool on_wall_collision(const std::vector<Rectangle>& player_positions, std::function<void(int, double)> damage_player);
    virtual bool on_player_collision(int hit_player, const std::vector<Rectangle>& player_positions, std::function<void(int, double)> damage_player);
    virtual bool on_no_collision();

    virtual double get_velocity() const;

    virtual void render(RenderList& list) const;

// private:

    int ticks_left = 40;
private:

    bool count_down_life();

    virtual const char* bullet_image_name() const;
};

#endif