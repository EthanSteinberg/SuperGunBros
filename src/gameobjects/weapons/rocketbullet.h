#ifndef ROCKET_BULLET_H_INCLUDED
#define ROCKET_BULLET_H_INCLUDED

#include "bullet.h"

class RocketBullet : public Bullet {
public:

    virtual bool on_wall_collision(const std::vector<Rectangle>& player_positions, std::function<void(int, double)> damage_player) override;
    virtual bool on_player_collision(int hit_player, const std::vector<Rectangle>& player_positions, std::function<void(int, double)> damage_player) override;
    virtual bool on_no_collision() override;

    virtual double get_velocity() const override;

    virtual bool create_explosion_after_destruction() const override;

private:
    virtual const char* bullet_image_name() const override;

    void perform_explosion(const std::vector<Rectangle>& player_positions, std::function<void(int, double)> damage_player) const;
};

#endif