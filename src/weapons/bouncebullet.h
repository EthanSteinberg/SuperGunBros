#ifndef BOUNCE_BULLET_H_INCLUDED
#define BOUNCE_BULLET_H_INCLUDED

#include "bullet.h"

class BounceBullet : public Bullet {
public:

    virtual bool on_wall_collision(const std::vector<Rectangle>& player_positions, std::function<void(int, double)> damage_player, bool free_horizontal, bool free_vertical);
    virtual bool on_player_collision(int hit_player, const std::vector<Rectangle>& player_positions, std::function<void(int, double)> damage_player);
    virtual bool on_no_collision();

    virtual double get_velocity() const;

private:

    int bounces_left = 5;

    virtual const char* bullet_image_name() const;
};

#endif