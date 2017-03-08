#ifndef PIERCE_BULLET_H_INCLUDED
#define PIERCE_BULLET_H_INCLUDED

#include "bullet.h"

class PierceBullet : public Bullet {
public:

    virtual bool on_wall_collision(const std::vector<Rectangle>& player_positions, std::function<void(int, double)> damage_player) override;
    virtual bool on_player_collision(int hit_player, const std::vector<Rectangle>& player_positions, std::function<void(int, double)> damage_player) override;
    virtual bool on_no_collision() override;

    virtual double get_velocity() const override;

    virtual bool pierce_special_effect() const override {
        return true;
    }

private:
    virtual const char* bullet_image_name() const override;

    enum class PierceBulletState {
        BEFORE_WALL,
        IN_WALL,
    };

    int num_pierces_left = 2;

    PierceBulletState state = PierceBulletState::BEFORE_WALL;
};

#endif