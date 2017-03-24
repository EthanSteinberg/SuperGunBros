#include "bouncebullet.h"

#include "soundthread.h"

#include <cmath>

bool BounceBullet::on_wall_collision(const std::vector<Rectangle>&, std::function<void(int, double)>, bool free_horizontal, bool free_vertical, SoundThread& sounds) {
    if (ticks_left-- < 0) {
        return true;
    }

    if (free_vertical) {
        angle = -angle;
    } else if (free_horizontal) {
        angle = M_PI - angle;
    } else {
        // Nothing is free, so turn around
        angle = M_PI + angle;
    }

    sounds.play_sound("../assets/sound/bounce.wav");

    return false;
}

bool BounceBullet::on_player_collision(int hit_player, const std::vector<Rectangle>&, std::function<void(int, double)> damage_player) {
    if ((unsigned int)hit_player == player_owner && ticks_left > BOUNCE_BULLET_LIFE - 20) {
        return false;
    }

    damage_player(hit_player, 10);
    return true;
}

bool BounceBullet::on_no_collision() {
    ticks_left--;
    return false;
}

double BounceBullet::get_velocity() const {
    return 10;
}

const char* BounceBullet::bullet_image_name() const {
    return "ricochet-round";
}