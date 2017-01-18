#include "gamescreen.h"

#include "gameoverscreen.h"

#include <string>
#include <cmath>

#define DRAG_COEF 0.08
#define JUMP_STR 0.6
#define GRAVITY 0.04
#define X_ACCEL 0.03
#define JUMP_DUR 12
#define SIGMA 0.0001
#define BOOST_STR 0.5
#define BOOST_DUR 10
#define PLAYER_WIDTH 0.5
#define PLAYER_HEIGHT 0.5

GameScreen::GameScreen(const std::vector<PlayerInfo> &infos){

    // Initialize the player state
    for (unsigned int i = 0; i < infos.size(); i++) {
        const auto& info = infos[i];
        Player player(0, 10, info);
        switch (i) {
            case 0:
                player.state.x = 2.25;
                break;

            case 1:
                player.state.x = 19.75;
                break;
        }
        players.push_back(player);
    }

    // Initialize the level
        std::vector<std::vector<Terrain>> grid;// = new std::vector<std::vector<Terrain>>;

        // Set up sizes. (HEIGHT x WIDTH)
        grid.resize(22);
        for (int i = 0; i < 22; ++i)
            grid[i].resize(12);

    for (int x = 0; x < 22; x++) {
        for (int y = 0; y < 12; y++) {

            if (x == 0 || x == 21 || y == 0 || y == 11) {
                // Add borders around the map
                grid[x][y] = Terrain ::GROUND;
            } else if (x >=9 && x < 15 && y == 7){
                grid[x][y] = Terrain ::GROUND;
            } else {
                grid[x][y] = Terrain ::OPEN;
            }

        }
    }

    level = Level(22, 12, grid);

}

void GameScreen::render(RenderList& list, double mouseX, double mouseY) {

	level.draw(list);

	// Render the players.

	for (const auto& player : players) {
        player.render(list);
	}

	// Render the bullets
	for (const auto& bullet: bullets) {
		list.add_image("bullet", bullet.x * 60 - 5, bullet.y * 60 - 5);
	}
}

const double player_height = 1.9;

std::unique_ptr<Screen> GameScreen::update(GLFWwindow* window) {
    for (unsigned int i = 0; i < players.size(); i++) {
        auto &player = players[i];

        player.update(window);

        bool firing_bullet = false;
        bool attempting_jump = false;

        double accel = 0;

        //Threshold the motion so that you aren't creeping around at slow speed
        float x_thresh = (fabs(player.state.input.ls.x) > 0.3) ? player.state.input.ls.x : 0;
        accel += x_thresh * X_ACCEL;

        //Directional aiming, but if player isn't aiming, aim flat in direction of motion.
        if (fabs(player.state.input.rs.x) < 0.3 && fabs(player.state.input.rs.y) < 0.3) {

            //Original code
            //This line is a travesty and I'm happy about that (direction of motion aim, keep direction if not moving)
            //int right = (dx > 0) ? 1 : (dx < 0) ? 0 : (player.facing_right) ? 1 : 0;
            //player.gun_angle = M_PI - right * M_PI;

            //This code changes it to the left stick controlling aim in the absence of right stick controls
            //Still reverts to flat aim in previous direction if no input is provided
            if (fabs(player.state.input.ls.x) < 0.3 && fabs(player.state.input.ls.y) < 0.3) {
                player.state.gun_angle = 0; // TODO: This should be based on last movement!
            } else {
                player.state.gun_angle = atan2(player.state.input.ls.y, player.state.input.ls.x);
            }
        } else {
            player.state.gun_angle = atan2(player.state.input.rs.y, player.state.input.rs.x);
        }


        //Checking inputs for later calculations
        attempting_jump = player.state.input.buttons[ButtonName::LT];
        firing_bullet = player.state.input.buttons[ButtonName::RT];

        //fprintf(stdout, "%d\n", player.state.input.buttons[ButtonName::RT]); //TODO determine why sometimes the gun won't fire if you hold it


        //X-Speed considerations for stopping and turning around on the ground
        if (player.state.grounded) {
            if (accel == 0) {
                player.state.dx = player.state.dx / 4;
                if (fabs(player.state.dx) < 0.2 * X_ACCEL) {
                    player.state.dx = 0;
                }
            } else if (fabs(player.state.dx + accel) < fabs(player.state.dx)) {
                player.state.dx = (player.state.dx / 4) + accel;
            } else {
                player.state.dx += accel;
            }
        } else {
            //X-Speed considerations for aerial motion
            player.state.dx += accel;
        }

        //Can't accelerate into the wall we're already on
        if(player.state.pushing_wall && player.state.pushing_wall < 0 == player.state.dx < 0){
            player.state.dx = 0;
        }

        //Speed cap for horizontal motion
        //TODO figure out if we want to alter or remove the cap when not grounded
        //if (fabs(player.dx) > MAX_X_SPEED){
        //    player.dx = (player.dx > 0) ? MAX_X_SPEED : -MAX_X_SPEED;
        //}

        //Jumping/Gravity logic
        //Grounded
        if(player.state.grounded){

            //player.state.dy = 0;
            player.state.ticks_left_jumping = JUMP_DUR;

            //Standard-Jump
            if(attempting_jump){
                player.state.jumping = true;
                player.state.dy -= 1 * JUMP_STR;
            }

            //Wall-Cling
        } else if (player.state.pushing_wall){

            //Reset Jumps
            player.state.ticks_left_jumping = JUMP_DUR/2;

            //Half speed fall with friction
            //TODO revisit formula
            player.state.dy = (player.state.dy + GRAVITY)/2;

            //Wall-Jump
            if(attempting_jump){ //TODO handy controls for wall-jumping
                int dir = -player.state.pushing_wall;
                player.state.jumping = true;
                player.state.pushing_wall = 0;
                player.state.dy -= 1 * JUMP_STR * cos(M_PI/4);
                player.state.dx = dir * JUMP_STR * cos(M_PI/4);
                player.state.x += dir * 2 * SIGMA; //Get off that wall
            }

            //Aerial Logic
        } else {
            //Continued-Jump
            if(player.state.jumping){
                if(attempting_jump && player.state.ticks_left_jumping > 0){
                    player.state.ticks_left_jumping--;
                    //To smoothly counteract the falling motion
                    player.state.dy -= GRAVITY; // * (player.ticks_left_jumping/(double)JUMP_DIR);

                } else {
                    player.state.jumping = false;
                    player.state.ticks_left_jumping = 0;
                }
            }
            //Gravity always has an effect in the air.
            player.state.dy += GRAVITY;
            //if (fabs(player.dy) > MAX_Y_SPEED){
            //    player.dy = (player.dy > 0) ? MAX_Y_SPEED : -MAX_Y_SPEED;
            //}
        }

        //Drag Calculations
        player.state.dx -= player.state.dx * DRAG_COEF;
        player.state.dy -= player.state.dy * DRAG_COEF;

        //Collision tracking super basic

        double low = 0.0;
        double mid = 0.5;
        double high = 1.0;

        if(is_colliding_with_ground(player.state.x + player.state.dx, player.state.y + player.state.dy, 0.5, player_height)){
            while(high - low > SIGMA){
                if(is_colliding_with_ground(player.state.x + mid * player.state.dx, player.state.y + mid * player.state.dy, 0.5, player_height)){
                    high = mid;
                } else {
                    low = mid;
                }
                mid = low + (high - low)/2;
            }
            //TODO logic that slides along colliding surface
            player.state.x += low * player.state.dx;
            player.state.y += low * player.state.dy;
        } else {
            player.state.x += player.state.dx;
            player.state.y += player.state.dy;
        }

        if(is_colliding_with_ground(player.state.x, player.state.y + SIGMA, 0.5, player_height)){
            player.state.grounded = true;
            player.state.dy = 0;
        } else {
            player.state.grounded = false;
        }

        if(is_colliding_with_ground(player.state.x, player.state.y - SIGMA, 0.5, player_height)){
            player.state.roofed = true;
            player.state.jumping = false;
            player.state.dy = 0;
        } else {
            player.state.roofed = false;
        }

        if(is_colliding_with_ground(player.state.x + SIGMA, player.state.y, 0.5, player_height)){
            player.state.pushing_wall = 1;
            //player.state.jumping = false;
            player.state.dx = 0;
        } else if (is_colliding_with_ground(player.state.x - SIGMA, player.state.y, 0.5, player_height)) {
            player.state.pushing_wall = -1;
            //player.state.jumping = false;
            player.state.dx = 0;
        } else {
            player.state.pushing_wall = false;
        }



        //Make motions as necessary


        //Bullet logic
        if (player.state.ticks_till_next_bullet > 0) {
            player.state.ticks_till_next_bullet --;
        } else if (firing_bullet) {
            player.state.ticks_till_next_bullet = 15;

            Bullet next_bullet = player.spawn_bullet();
            next_bullet.player_owner = i;

            bullets.push_back(next_bullet);
        }

    }

    std::vector<Bullet> next_bullets;

    // Update all the bullets
    for (auto& bullet : bullets) {
        bullet.x += bullet.x_vel;
        bullet.y += bullet.y_vel;


        if (is_colliding_with_ground(bullet.x, bullet.y, 10/60.0, 10/60.0)) {
            // This bullet hit the ground.
            continue;
        }

        bool hit_a_player = false;
        bool hit_a_bullet = false;

        for (unsigned int i = 0; i < players.size(); i++) {
            auto& player = players[i];

            if (i == bullet.player_owner) {
                continue; // Don't intersect with self
            }

            if (rect_rect_colliding(bullet.x, bullet.y, 10/60.0, 10/60.0, player.state.x, player.state.y, 0.5, player_height)) {
                hit_a_player = true;
                player.state.health -= 10;

                if (player.state.health == 0) {
                    auto& otherPlayer = players[1 - i];
                    return std::make_unique<GameOverScreen>(otherPlayer.info);
                }
            }
        }

        //Not a good way to check for bullet collisions, but it's here for now.
        //TODO make a fullproof method that doesn't risk destroying a single bullet and not the other one
        //TODO make this all part of the projectile class logic to allow for some bullets to beat others etc.
        for (auto& bullet2 : bullets) {
            if (&bullet2 != &bullet && rect_rect_colliding(bullet.x, bullet.y, 10/60.0, 10/60.0, bullet2.x, bullet2.y, 10/60.0, 10/60.0)){
                hit_a_bullet = true;
            }
        }



        if (hit_a_player || hit_a_bullet) {
            continue;
        }

        next_bullets.push_back(bullet);
    }


    bullets = next_bullets;

    return nullptr;
}

std::unique_ptr<Screen> GameScreen::on_click(int button, int action, double mouseX, double mouseY) {
    return nullptr;
}

std::unique_ptr<Screen> GameScreen::on_key(int key, int action) {
	return nullptr;
}

bool GameScreen::rect_rect_colliding(double x1, double y1, double width1, double height1, double x2, double y2, double width2, double height2) {
	double total_width = width1 + width2;
	double total_height = height1 + height2;

	return (fabs(x1 - x2) < total_width / 2 && fabs(y1 - y2) < total_height / 2);
}

bool GameScreen::is_colliding_with_ground(double x, double y, double width, double height) {
	return (
		level.is_obstacle((int)(x - width/2), (int)(y - height/2)) ||
        level.is_obstacle((int)(x + width/2), (int)(y - height/2)) ||
        level.is_obstacle((int)(x - width/2), (int)(y + height/2)) ||
        level.is_obstacle((int)(x + width/2), (int)(y + height/2)));
}