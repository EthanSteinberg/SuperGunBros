#ifndef FLAME_H_INCLUDED
#define FLAME_H_INCLUDED

#include "gameobjects/weapons/gun.h"

class Flame : public Gun {
public:
    virtual bool in_front() const override;

    virtual int ticks_between_shots() const override{
        return 0;
    }

    virtual const char* shoot_sound() override {
        return nullptr;
    }

    virtual const char* holding_shoot_sound() override{
        return "../assets/sound/flame.wav";
    }

    virtual int initial_ammo() const override{
        return 100;
    }

    virtual std::vector<std::unique_ptr<Bullet>> spawn_bullets(double gun_angle) const override;

private:
    virtual double gun_rotation_x() const override;
    virtual double gun_rotation_y() const override;

    virtual double gun_offset_x() const override;
    virtual double gun_offset_y() const override;

    virtual double barrel_offset_x() const override;
    virtual double barrel_offset_y() const override;

    virtual double grip1_dx() const override;
    virtual double grip1_dy() const override;

    virtual double grip2_dx() const override;
    virtual double grip2_dy() const override;

    virtual const char* gun_image_name() const override;

    virtual std::unique_ptr<Bullet> create_initial_bullet() const override;

    virtual double barrel_length() const override;
};

#endif