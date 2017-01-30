#include "gamescreen.h"

#include "gameoverscreen.h"
#include "rocket.h"
#include <string>
#include <cmath>

//#define DRAG_COEF 0.125
//#define JUMP_STR 15
//#define GRAVITY 0.4
//#define X_ACCEL 0.25
//#define JUMP_DUR 15
//#define SIGMA 0.001
//#define BOOST_STR 7
//#define BOOST_DUR 10

#define DRAG_COEF 0.1
#define JUMP_STR 20
#define GRAVITY 0.5
#define X_ACCEL 0.5
#define JUMP_DUR 15
#define SIGMA 0.001
#define BOOST_STR 5
#define BOOST_DUR 10

GameScreen::GameScreen(const std::vector<PlayerInfo> &infos, const Level& a_level): level(a_level) {

    // Initialize the player state
    for (unsigned int i = 0; i < infos.size(); i++) {
        const auto& info = infos[i];
        Player player(0, 300, info);
        switch (i) {
            case 0:
                player.state.pos.x = 200;
                break;

            case 1:
                player.state.pos.x = 1100;
                break;
        }
        players.push_back(player);
    }
}

void GameScreen::render(RenderList& list, double mouseX, double mouseY) {

    list.add_image("background", 0, 0);

    level.render(list);

    for (const auto& explosion: explosions) {
        explosion.render(list);
    }

    // Render the weapon boxes
    for (const auto& box: boxes) {
        box.render(list);
    }

	// Render the players.

	for (const auto& player : players) {
        player.render(list);
	}

	// Render the bullets
	for (const auto& bullet: bullets) {
        list.translate(bullet.pos.x, bullet.pos.y);
        double bullet_angle = atan2(bullet.y_vel, bullet.x_vel);
        list.rotate(bullet_angle);
		list.add_image("bullet", -bullet.pos.width/2, -bullet.pos.height/2);
        list.rotate(-bullet_angle);
        list.translate(-bullet.pos.x, -bullet.pos.y);
	}

    list.add_image("black", 0, 660, 1280, 60);

    for (unsigned int i = 0; i < players.size(); i++) {
        const auto& player = players[i];

        int x_offset = 0;

        if (i == 0) {
            x_offset = 275/2.0;
        } else {
            x_offset = 1280 - 275/2.0;
        }

        list.translate(x_offset, 0);

        Rectangle info_box(0, 690, 275, 60);
        list.add_rect("white", info_box);
        list.add_outline("black", info_box);

        const char* life_color = nullptr;

        switch (player.info.color) {
            case PlayerColor::RED:
                life_color = "redLife";
                break;

            case PlayerColor::YELLOW:
                life_color = "yellowLife";
                break;

            default:
                std::cout<<"Invalid player color\n";
                exit(-1);
        }

        const char* dead_color = "deadLife";

        for (int i = 0; i < 3; i++) {
            Rectangle life_box(-100 + i * 45, 690, 30, 30);
            list.add_rect(player.state.lives_left > i ? life_color : dead_color, life_box);
        }

        {
            list.translate(40, 690);
            player.state.gun->render_large(list);
            list.translate(-40, -690);
        }

        if (player.state.ammo_left != -1) {
            list.add_number(80, 705, player.state.ammo_left);
        } else {
            list.add_image("inf", 80, 705 - 19);
        }




        list.translate(-x_offset, 0);
    }
}

std::unique_ptr<Screen> GameScreen::update(GLFWwindow* window) {

    std::vector<Explosion> next_explosions;

    for (auto& explosion: explosions) {
        explosion.update();

        if (!explosion.is_done()) {
            next_explosions.push_back(explosion);
        }
    }

    explosions = next_explosions;

    for (unsigned int i = 0; i < players.size(); i++) {
        auto &player = players[i];

        player.update(window);

        double accel = 0;

        //Threshold the motion so that you aren't creeping around at slow speed
        float x_thresh = (fabs(player.state.input.ls.x) > 0.3) ? player.state.input.ls.x : 0;
        accel += x_thresh * X_ACCEL;

        //Directional aiming, but if player isn't aiming, aim flat in direction of motion.
        if (fabs(player.state.input.rs.x) > 0.3 || fabs(player.state.input.rs.y) > 0.3) {
            player.state.gun_angle = atan2(player.state.input.rs.y, player.state.input.rs.x);
        }

        //Checking inputs for later calculations
        bool attempting_jump = player.state.input.buttons[ButtonName::LT];
        bool firing_bullet = player.state.input.buttons[ButtonName::RT];

        bool attemping_boost = player.state.input.buttons[ButtonName::LB];

        bool attempting_pickup = player.state.input.buttons[ButtonName::X];

        if (attempting_pickup) {
            auto potential_gun = attempt_pick_up(player.state.pos);

            if (potential_gun != nullptr) {
                player.state.gun = potential_gun;
                player.state.ammo_left = 10;
            }
        }

        if (attemping_boost && player.state.fuel_left > 0) {
            player.state.dy -= BOOST_STR;
            player.state.fuel_left -= 1.0/BOOST_DUR;
            player.state.boosting = true;

            if (player.state.fuel_left < 0) {
                player.state.fuel_left = 0;
            }
        } else {
            player.state.boosting = false;
        }

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
        if (player.state.pushing_wall && (player.state.pushing_wall < 0) == (player.state.dx < 0)) {
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

            player.state.fuel_left = std::min(player.state.fuel_left + 1.0/BOOST_DUR, 1.0);

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
                player.state.pos.x += dir * 2 * SIGMA; //Get off that wall
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

        auto would_collide = [&](double dx, double dy){
            return would_hit_ground(player.state.pos.offset(dx, dy)); //|| would_hit_box(player.state.pos.offset(dx, dy));
        };

        if (would_collide(player.state.dx, player.state.dy)){
            double low = 0.0;
            double high = 1.0;
            while(high - low > SIGMA/10){
                double mid = low + (high - low)/2;
                if (would_collide(mid * player.state.dx, mid * player.state.dy)) {
                    high = mid;
                } else {
                    low = mid;
                }
            }
            //TODO logic that slides along colliding surface
            player.state.pos.x += low * player.state.dx;
            player.state.pos.y += low * player.state.dy;
        } else {
            player.state.pos.x += player.state.dx;
            player.state.pos.y += player.state.dy;
        }

        if (would_collide(0, SIGMA)){
            player.state.grounded = true;
            player.state.dy = 0;
        } else {
            player.state.grounded = false;
        }

        if (would_collide(0, -SIGMA)) {
            player.state.roofed = true;
            player.state.jumping = false;
            player.state.dy = 0;
        } else {
            player.state.roofed = false;
        }

        if (would_collide(SIGMA, 0)) {
            if(accel > 0) {
                player.state.pushing_wall = 1;
            }
            player.state.dx = 0;
        } else if (would_collide(-SIGMA, 0) && accel < 0) {
            if(accel < 0){
                player.state.pushing_wall = -1;
            }
            player.state.dx = 0;
        } else {
            player.state.pushing_wall = false;
        }

        //Bullet logic
        if (player.state.ticks_till_next_bullet > 0) {
            player.state.ticks_till_next_bullet --;
        } else if (firing_bullet) {
            player.state.ticks_till_next_bullet = 15;

            Bullet next_bullet = player.spawn_bullet();
            next_bullet.player_owner = i;

            if (player.state.ammo_left != -1) {
                player.state.ammo_left --;

                if (player.state.ammo_left <= 0) {
                    player.state.gun = std::make_shared<Pistol>();
                    player.state.ammo_left = -1;
                }
            }

            bullets.push_back(next_bullet);
        }

    }

    std::vector<Bullet> next_bullets;

    // Update all the bullets
    for (auto& bullet : bullets) {
        bullet.pos.x += bullet.x_vel;
        bullet.pos.y += bullet.y_vel;


        bool hit_something = false;

        if (would_hit_ground(bullet.pos)) {
            // This bullet hit the ground.
            hit_something = true;
        }

        for (unsigned int i = 0; i < players.size(); i++) {
            auto& player = players[i];

            if (i == bullet.player_owner) {
                continue; // Don't intersect with self
            }

            if (bullet.pos.colliding_with(player.state.pos)) {
                hit_something = true;
                auto res = damage_player(i, 10);

                if (res != nullptr) {
                    return res;
                }
            }
        }

        for (unsigned int i = 0; i < boxes.size(); i++) {
            WeaponBox& box = boxes[i];

            if (!box.opened && bullet.pos.colliding_with(box.pos)) {
                hit_something = true;
                box.opened = true;
            }
        }

        if (hit_something) {
            // throw away the bullet if something is hit.

            if (bullet.is_explosive_bullet) {
                explosions.push_back(Explosion(bullet.pos.x, bullet.pos.y));


                for (unsigned int i = 0; i < players.size(); i++) {
                    auto& player = players[i];

                    // TODO: Actual circle, rect intersection test
                    if (bullet.pos.location().dist(player.state.pos.location()) < (EXPLOSION_RADIUS + player.state.pos.width)) {
                        hit_something = true;
                        auto res = damage_player(i, 15);

                        if (res != nullptr) {
                            return res;
                        }
                    }
                }
            }

            continue;
        }

        next_bullets.push_back(bullet);
    }

    for (auto& box: boxes) {
        Rectangle new_pos = box.pos.offset(0, 1);
        if (!would_hit_player(new_pos) && !would_hit_ground(new_pos)) {
            box.pos = new_pos;;
        }
    }

    bullets = next_bullets;

    ticks_till_next_box--;

    if (ticks_till_next_box <= 0) {
        ticks_till_next_box = 600;
        for (int i = 0; i < 10; i++) {
            // Only try to spawn a box a maximum of 10 times ...
            Point possible_location = level.get_random_box_spawn_location(gen);

            Rectangle possible_box(possible_location.x, possible_location.y, 40, 40);

            if (!would_collide_or_fall_on_any(possible_box)) {
                boxes.push_back(WeaponBox{possible_box, false});
                break;
            }
        }
    }

    return nullptr;
}

std::unique_ptr<Screen> GameScreen::damage_player(int player_index, int damage) {
    auto& player = players[player_index];

    player.state.health -= damage;

    if (player.state.health <= 0) {
        player.state.lives_left--;

        player.state.health = MAX_HEALTH;
        player.state.pos.y = 300;

        switch (player_index) {
            case 0:
                player.state.pos.x = 200;
                break;

            case 1:
                player.state.pos.x = 1100;
                break;
        }


        if (player.state.lives_left == 0) {
            auto& otherPlayer = players[1 - player_index];
            return std::make_unique<GameOverScreen>(otherPlayer.info);
        }
    }

    return nullptr;
}

std::unique_ptr<Screen> GameScreen::on_click(int button, int action, double mouseX, double mouseY) {
    return nullptr;
}

std::unique_ptr<Screen> GameScreen::on_key(int key, int action) {
	return nullptr;
}

bool GameScreen::would_hit_ground(const Rectangle& rect) const {
    return level.colliding_with(rect);
}

bool GameScreen::would_hit_player(const Rectangle& rect) const {
    for (const auto& player: players) {
        if (player.state.pos.colliding_with(rect)) {
            return true;
        }
    }

    return false;
}

bool GameScreen::would_hit_box(const Rectangle& rect) const {
    for (const auto& box: boxes) {
        if (!box.opened && box.pos.colliding_with(rect)) {
            return true;
        }
    }

    return false;
}

bool GameScreen::would_collide_or_fall_on_any(const Rectangle& rect) const {
    for (const auto& box : boxes) {
        if (fabs(rect.x - box.pos.x) < (rect.width + box.pos.width)) {
            return true;
        }
    }

    return would_hit_ground(rect) || would_hit_player(rect);
}

std::shared_ptr<Gun> GameScreen::attempt_pick_up(const Rectangle& rect) {
    std::vector<WeaponBox> next_boxes;

    std::shared_ptr<Gun> result = nullptr;

    for (const auto& box: boxes) {
        if (box.opened && box.pos.colliding_with(rect)) {
            result = std::make_shared<Rocket>();
        } else {
            next_boxes.push_back(box);
        }
    }

    boxes = next_boxes;

    return result;
}