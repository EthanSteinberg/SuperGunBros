#ifndef FLAME_H_INCLUDED
#define FLAME_H_INCLUDED

#include "gun.h"

class Flame : public Gun {
public:
    virtual bool in_front() const;

    virtual int ticks_between_shots() const {
        return 0;
    }

    virtual const char* shoot_sound() {
        return nullptr;
    }

    virtual const char* holding_shoot_sound() {
        return "../assets/sound/flame.wav";
    }

    virtual int initial_ammo() const {
        return 100;
    }

    virtual std::vector<std::unique_ptr<Bullet>> spawn_bullets(double gun_angle) const;

private:
    virtual double gun_rotation_x() const;
    virtual double gun_rotation_y() const;

    virtual double gun_offset_x() const;
    virtual double gun_offset_y() const;

    virtual double barrel_offset_x() const;
    virtual double barrel_offset_y() const;

    virtual double grip1_dx() const;
    virtual double grip1_dy() const;

    virtual double grip2_dx() const;
    virtual double grip2_dy() const;

    virtual const char* gun_image_name() const;

    virtual std::unique_ptr<Bullet> create_initial_bullet() const;

    virtual double barrel_length() const;
};

#endif