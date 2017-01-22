#ifndef GAME_SCREEN_H_INCLUDED
#define GAME_SCREEN_H_INCLUDED

#include "screen.h"
#include <vector>
#include "player.h"
#include "rectangle.h"
#include "level.h"

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

    bool would_collide_with_ground(const Rectangle& rect, double dx, double dy) const;
};

#endif