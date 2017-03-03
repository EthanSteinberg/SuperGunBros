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

struct PlayerSpawnTimer {
    int ticks_left_till_spawn;
    int player_index;
};

struct PierceEffectData {
    uint64_t bullet_id;
    Point start;
    Point end;
    bool has_end_yet;
    int ticks_left;
};

class GameScreen : public Screen {
public:
    GameScreen(const std::vector<PlayerInfo>& players, const Level& level);

    void render(RenderList& list) const;
    std::unique_ptr<Screen> update(const std::map<int, inputs>& joystick_inputs, const std::map<int, inputs>& last_inputs, SoundThread& thread);
private:
    Level level;

    std::vector<Player> players;
    std::vector<std::unique_ptr<Bullet>> bullets;

    std::vector<WeaponBox> boxes;

    std::vector<Explosion> explosions;

    std::vector<PierceEffectData> pierce_effects;

    std::mt19937 gen;

    Camera camera;

    bool would_hit_ground(const Rectangle& rect) const;

    bool would_hit_player(const Rectangle& rect) const;
    bool would_hit_box(const Rectangle& rect) const;

    std::unique_ptr<Gun> attempt_pick_up(const Rectangle& rect);

    void damage_player(int player_index, double damage, int shooter_index);

    bool game_over = false;
};

#endif