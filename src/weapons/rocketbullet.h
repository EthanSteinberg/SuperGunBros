#ifndef ROCKET_BULLET_H_INCLUDED
#define ROCKET_BULLET_H_INCLUDED

#include "bullet.h"

class RocketBullet : public Bullet {
public:

    virtual bool on_wall_collision(const std::vector<Rectangle>& player_positions, std::function<void(int, int)> damage_player) const;
    virtual bool on_player_collision(int hit_player, const std::vector<Rectangle>& player_positions, std::function<void(int, int)> damage_player) const;

    virtual double get_velocity() const;

    virtual bool create_explosion_after_destruction() const;

private:
    virtual const char* bullet_image_name() const;

    void perform_explosion(const std::vector<Rectangle>& player_positions, std::function<void(int, int)> damage_player) const;
};

#endif