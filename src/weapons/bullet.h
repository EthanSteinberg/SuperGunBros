#ifndef BULLET_H_INCLUDED
#define BULLET_H_INCLUDED

#include "rectangle.h"
#include <vector>
#include <functional>
#include "renderlist.h"

class Bullet {
public:
    Bullet();

    virtual ~Bullet();

    unsigned int player_owner;

    Rectangle pos;

    double angle;

    void render(RenderList& list) const;

    // Return true if the bullet should be marked dead
    virtual bool on_wall_collision(const std::vector<Rectangle>& player_positions, std::function<void(int, int)> damage_player) const = 0;
    virtual bool on_player_collision(int hit_player, const std::vector<Rectangle>& player_positions, std::function<void(int, int)> damage_player) const = 0;

    virtual double get_velocity() const = 0;

    virtual bool create_explosion_after_destruction() const;

private:
    virtual const char* bullet_image_name() const = 0;
};

#endif