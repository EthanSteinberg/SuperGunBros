#ifndef GUN_H_INCLUDED
#define GUN_H_INCLUDED

#include "renderlist.h"
#include "bullet.h"
#include <memory>

class Gun {
public:
    void render(RenderList& list, double gun_angle) const;

    double grip1_x(double gun_angle) const;
    double grip1_y(double gun_angle) const;

    double grip2_x(double gun_angle) const;
    double grip2_y(double gun_angle) const;

    virtual bool in_front() const = 0;

    virtual std::vector<std::unique_ptr<Bullet>> spawn_bullets(double gun_angle) const;

    void render_large(RenderList& list) const;

    virtual int ticks_between_shots() const {
        return 15;
    }

    virtual int initial_ammo() const {
        return 10;
    }

    virtual ~Gun();

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

    virtual const char* gun_image_name() const = 0;

    virtual std::unique_ptr<Bullet> create_initial_bullet() const = 0;
};


std::unique_ptr<Gun> create_gun(const std::string& gun_name);

#endif