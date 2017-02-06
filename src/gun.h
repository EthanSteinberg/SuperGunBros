#ifndef GUN_H_INCLUDED
#define GUN_H_INCLUDED

#include "renderlist.h"
#include "bullet.h"

class Gun {
public:
    void render(RenderList& list, double gun_angle) const;

    double grip1_x(double gun_angle) const;
    double grip1_y(double gun_angle) const;

    double grip2_x(double gun_angle) const;
    double grip2_y(double gun_angle) const;

    virtual bool in_front() const = 0;

    Bullet spawn_bullet(double gun_angle) const;


    virtual void render_large(RenderList& list) const = 0;

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


    virtual const char* gun_image_name() const = 0;

    virtual bool has_explosive_bullets() const = 0;
};

#endif