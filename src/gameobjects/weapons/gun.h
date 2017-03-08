#ifndef GUN_H_INCLUDED
#define GUN_H_INCLUDED

#include "rendering/renderlist.h"
#include "bullet.h"
#include <memory>
#include <gameobjects/gameobject.h>

class Gun : public GameObject{
public:
    void render(RenderList& list) const;
    void render_aim(RenderList& list, double gun_angle) const;

    void update() {};


    double grip1_x(double gun_angle) const;
    double grip1_y(double gun_angle) const;

    double grip2_x(double gun_angle) const;
    double grip2_y(double gun_angle) const;

    virtual const char* shoot_sound() {
        return "../assets/sound/shoot.wav";
    }

    virtual const char* holding_shoot_sound() {
        return nullptr;
    }

    virtual bool in_front() const = 0;

    virtual std::vector<std::unique_ptr<Bullet>> spawn_bullets(double gun_angle) const;

    virtual int ticks_between_shots() const {
        return 20;
    }

    virtual int initial_ammo() const = 0;
    virtual ~Gun();

    virtual const char* gun_image_name() const = 0;

private:
    virtual double gun_rotation_x() const = 0;
    virtual double gun_rotation_y() const = 0;

    virtual double gun_offset_x() const = 0;
    virtual double gun_offset_y() const = 0;

    virtual double barrel_offset_x() const = 0;
    virtual double barrel_offset_y() const = 0;

    virtual double grip1_dx() const = 0;
    virtual double grip1_dy() const = 0;

    virtual double grip2_dx() const = 0;
    virtual double grip2_dy() const = 0;

    virtual double barrel_length() const {
        return 20;
    }

    virtual std::unique_ptr<Bullet> create_initial_bullet() const = 0;
};


std::unique_ptr<Gun> create_gun(const std::string& gun_name);

#endif