#ifndef PISTOL_H_INCLUDED
#define PISTOL_H_INCLUDED

#include "gun.h"

class Pistol : public Gun {
public:
    virtual bool in_front() const;

    virtual void render_large(RenderList& list) const;

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

    bool has_explosive_bullets() const;
};

#endif