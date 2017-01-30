#ifndef GAME_SCREEN_H_INCLUDED
#define GAME_SCREEN_H_INCLUDED

#include "screen.h"
#include <vector>
#include "player.h"
#include "rectangle.h"
#include "level.h"
#include "weaponbox.h"
#include "explosion.h"

class GameScreen : public Screen {
public:
    GameScreen(const std::vector<PlayerInfo>& players, const Level& level);

    void render(RenderList& list, double mouseX, double mouseY);
    std::unique_ptr<Screen> update(GLFWwindow* window);
    std::unique_ptr<Screen> on_click(int button, int action, double mouseX, double mouseY);
    std::unique_ptr<Screen> on_key(int key, int action);

private:
    Level level;

    std::vector<Player> players;
    std::vector<Bullet> bullets;

    std::vector<WeaponBox> boxes;

    std::vector<Explosion> explosions;

    int ticks_till_next_box = 0;

    std::mt19937 gen;

    bool would_hit_ground(const Rectangle& rect) const;

    bool would_collide_or_fall_on_any(const Rectangle& rect) const;

    bool would_hit_player(const Rectangle& rect) const;
    bool would_hit_box(const Rectangle& rect) const;

    std::shared_ptr<Gun> attempt_pick_up(const Rectangle& rect);

    std::unique_ptr<Screen> damage_player(int player_index, int damage);
};

#endif