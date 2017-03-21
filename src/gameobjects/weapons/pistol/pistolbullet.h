#ifndef PISTOL_BULLET_H_INCLUDED
#define PISTOL_BULLET_H_INCLUDED

#include "gameobjects/weapons/bullet.h"

class PistolBullet : public Bullet {
public:

    virtual bool on_wall_collision(const std::vector<Rectangle>& player_positions, std::function<void(int, double)> damage_player) override;
    virtual bool on_player_collision(int hit_player, const std::vector<Rectangle>& player_positions, std::function<void(int, double)> damage_player) override;
    virtual bool on_no_collision() override;

    virtual double get_velocity() const override;

private:
    virtual const char* bullet_image_name() const override;
};

#endif