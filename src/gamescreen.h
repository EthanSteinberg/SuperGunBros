#ifndef GAME_SCREEN_H_INCLUDED
#define GAME_SCREEN_H_INCLUDED

#include "screen.h"
#include <vector>
#include "player.h"
#include "rectangle.h"
#include "level.h"
#include "weaponbox.h"
#include "explosion.h"
#include "camera.h"

class GameScreen : public Screen {
public:
    GameScreen(const std::vector<PlayerInfo>& players, const Level& level);

    void render(RenderList& list) const;
    std::unique_ptr<Screen> update(const std::map<int, inputs>& joystick_inputs, const std::map<int, inputs>& last_inputs);
private:
    Level level;

    std::vector<Player> players;
    std::vector<Bullet> bullets;

    std::vector<WeaponBox> boxes;

    std::vector<Explosion> explosions;

    int ticks_till_next_box = 0;

    std::mt19937 gen;

    Camera camera;

    bool would_hit_ground(const Rectangle& rect) const;

    bool would_collide_or_fall_on_any(const Rectangle& rect) const;

    bool would_hit_player(const Rectangle& rect) const;
    bool would_hit_box(const Rectangle& rect) const;

    std::shared_ptr<Gun> attempt_pick_up(const Rectangle& rect);

    std::unique_ptr<Screen> damage_player(int player_index, int damage);
};

#endif