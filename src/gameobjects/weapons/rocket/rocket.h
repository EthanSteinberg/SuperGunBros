#ifndef ROCKET_H_INCLUDED
#define ROCKET_H_INCLUDED

#include "gameobjects/weapons/gun.h"

class Rocket : public Gun {
public:
    virtual bool in_front() const;
    virtual int initial_ammo() const {
        return 3;
    }

    virtual const char* shoot_sound() {
        return "../assets/sound/rocket_launcher.wav";
    }

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
};

#endif