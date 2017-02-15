#include "gamescreen.h"

#include "readyscreen.h"
#include <string>
#include <cmath>

#define DRAG_COEF 0.075
#define JUMP_STR 15
#define GRAVITY 0.75
#define X_ACCEL 0.5
#define JUMP_DUR 15
#define SIGMA 0.001
#define BOOST_STR 1
#define BOOST_DUR 180
#define WALL_FRICTION 0.2
#define WALL_GRACE 3
#define GROUND_GRACE 5

GameScreen::GameScreen(const std::vector<PlayerInfo> &infos, const Level& a_level): level(a_level), camera(level.width, level.height) {

    // Initialize the player state
    for (unsigned int i = 0; i < infos.size(); i++) {
        const auto& info = infos[i];
        Point p = level.get_player_spawn_locations()[(int)info.color];

        Player player(p.x, p.y, info);
        players.push_back(std::move(player));
    }
}

void GameScreen::render(RenderList& list) const {

    list.push();
    camera.transform(list);

    //list.add_image("background", 0, 0);

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
        bullet->render(list);
	}

    list.pop();

    // TODO: figure out what HUD elements are staying
    //list.add_image("black", 0, 660, 1280, 60);
//    for (unsigned int i = 0; i < players.size(); i++) {
//        const auto& player = players[i];
//
//        int x_offset = 0;
//
//        if (i == 0) {
//            x_offset = 275/2.0;
//        } else {
//            x_offset = 1280 - 275/2.0;
//        }
//
//        list.translate(x_offset, 0);
//
//        Rectangle info_box(0, 690, 275, 60);
//        list.add_rect("white", info_box);
//        list.add_outline("black", info_box);
//
//        std::string life_color = "life-" + get_color_name(player.info.color);
//
//        const char* dead_color = "deadLife";
//
//        for (int i = 0; i < 3; i++) {
//            Rectangle life_box(-100 + i * 45, 690, 30, 30);
//            list.add_rect(player.state.lives_left > i ? life_color : dead_color, life_box);
//        }
//
//        {
//            list.translate(40, 690);
//            player.state.gun->render_large(list);
//            list.translate(-40, -690);
//        }
//
//        if (player.state.ammo_left != -1) {
//            list.add_number(80, 705, player.state.ammo_left);
//        } else {
//            list.add_image("inf", 80, 705 - 19);
//        }
//
//        list.translate(-x_offset, 0);
//    }




    if (game_over) {
        std::string winning_color = "tie";

        for (const auto& player: players) {
            if (player.state.lives_left != 0) {
                winning_color = get_color_name(player.info.color);
            }
        }

        list.add_image("player-won-" + winning_color, (1280 - 1131)/2, 100);

        list.add_image("press-start-continue", (1280 - 1112)/2, 500);
    }
}

std::unique_ptr<Screen> GameScreen::update(const std::map<int, inputs>& all_joystick_inputs, const std::map<int, inputs>& all_last_inputs) {

    if (all_joystick_inputs.count(-1) == 1 && all_joystick_inputs.at(-1).buttons[ButtonName::L3] && !all_last_inputs.at(-1).buttons[ButtonName::L3]) {
        // This is the signal to reload the map.
        try {
            level = Level::load_all_levels()[level.index];
        } catch (const std::exception& ex) {
            std::cout<<"Got error while reloading level " << ex.what() <<std::endl;
        }

    }

    if (game_over) {
        // When there is no-one alive, any player should be able to continue
        int winning_player_index = -1;

        for (unsigned int i = 0; i < players.size(); i++) {
            const Player& player = players[i];
            if (!player.state.is_dead) {
                winning_player_index = i;
            }
        }

        for (unsigned int i = 0; i < players.size(); i++) {
            const Player& player = players[i];
            if (winning_player_index == (int) i || winning_player_index == -1) {
                inputs input = all_joystick_inputs.at(player.info.joystick_index);
                inputs last_input = all_last_inputs.at(player.info.joystick_index);

                if (input.buttons[ButtonName::START] && !last_input.buttons[ButtonName::START]) {
                    std::vector<int> joysticks;

                    for (const auto& item : all_joystick_inputs) {
                        joysticks.push_back(item.first);
                    }

                    return std::make_unique<ReadyScreen>(joysticks, level.index);
                }
            }
        }
    }

    std::vector<Point> player_positions;

    for (const Player& player : players) {
        if (!player.state.is_dead || player.state.ticks_until_spawn > DEATH_INVISIBLE_TIME) {
            player_positions.push_back(player.state.pos.location());
        }
    }

    camera.update(player_positions);

    for (auto& item : all_joystick_inputs) {
        if (item.second.buttons[BACK]) {
            std::vector<int> joysticks;

            for (const auto& item : all_joystick_inputs) {
                joysticks.push_back(item.first);
            }

            return std::make_unique<ReadyScreen>(joysticks, level.index);
        }
    }

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

        if (player.state.is_dead) {
            player.state.ticks_until_spawn --;

            if (player.state.ticks_until_spawn == 0) {
                player.state.is_dead = false;

                player.state.health = MAX_HEALTH;

                Point spawn_location = level.get_random_player_spawn_location(gen);

                player.state.pos.x = spawn_location.x;
                player.state.pos.y = spawn_location.y;

                player.set_gun(create_gun("pistol"));

                player.state.invincibility_ticks_left = DEATH_TIME;
            }
        } else {
            player.update();
            double accel = 0;

            int js = player.info.joystick_index;
            inputs current_inputs = all_joystick_inputs.at(js);
            inputs prev_inputs = all_last_inputs.at(js);

            //Threshold the motion so that you aren't creeping around at slow speed
            float x_thresh = (fabs(current_inputs.ls.x) > 0.3) ? current_inputs.ls.x : 0;
            accel += x_thresh * X_ACCEL;

            //Directional aiming, but if player isn't aiming, aim flat in direction of motion.
            if (fabs(current_inputs.rs.x) > 0.3 || fabs(current_inputs.rs.y) > 0.3) {
                player.state.gun_angle = atan2(current_inputs.rs.y, current_inputs.rs.x);
            }

            //Checking inputs for later calculations
            bool starting_jump = button_press(ButtonName::LT, current_inputs, prev_inputs);
            bool holding_jump = button_hold(ButtonName::LT, current_inputs, prev_inputs);

            bool pull_trigger = button_press(ButtonName::RT, current_inputs, prev_inputs);
            bool holding_trigger = button_hold(ButtonName::RT, current_inputs, prev_inputs);

            bool starting_boost = button_press(ButtonName::LB, current_inputs, prev_inputs);
            bool continuing_boost = button_hold(ButtonName::LB, current_inputs, prev_inputs) && player.state.boosting;

            bool attempting_pickup = current_inputs.buttons[ButtonName::X];

            if (attempting_pickup) {
                auto potential_gun = attempt_pick_up(player.state.pos);

                if (potential_gun != nullptr) {
                    player.set_gun(std::move(potential_gun));
                }
            }

            if ((continuing_boost && player.state.fuel_left > 0) || (starting_boost && player.state.fuel_left > 0.2)) {
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
            if (player.state.pushing_wall * player.state.dx > 0) {
                player.state.dx = 0;
            }

            auto would_collide = [&](double dx, double dy){
                return would_hit_ground(player.state.pos.offset(dx, dy)); //|| would_hit_box(player.state.pos.offset(dx, dy));
            };

            //Speed cap for horizontal motion
            //TODO figure out if we want to alter or remove the cap when not grounded
            //if (fabs(player.dx) > MAX_X_SPEED){
            //    player.dx = (player.dx > 0) ? MAX_X_SPEED : -MAX_X_SPEED;
            //}

            //Gravity logic
            //Grounded
            if(player.state.grounded){
                //player.state.dy = 0;
                player.state.fuel_left = std::min(player.state.fuel_left + 1.0/BOOST_DUR, 1.0);

                //Wall-Cling
            } else if (player.state.pushing_wall) {

                int max_up = -80;

                // Basically see if you can go up to get over the current obstacle.
                // This allows a simple mantle operation.
                bool can_go_up = false;

                for (int i = 0; i >= max_up; i -=5) {
                    if  (!would_collide(player.state.pushing_wall * SIGMA, i)) {
                        can_go_up = true;
                        break;
                    }
                }
                if (can_go_up) {
                    // Do a mantle operation
                    player.state.dy -= GRAVITY;
                } else {
                    //Half speed fall with friction
                    player.state.dy += GRAVITY;
                    //But not if you're jetpacking or jumping
                    if (!(player.state.boosting || player.state.jumping)) {
                        player.state.dy -= player.state.dy * WALL_FRICTION;
                    }
                }

                //Aerial Logic
            } else {

                //Continued-Jump
                if(player.state.jumping){
                    if(holding_jump && player.state.ticks_left_jumping > 0){
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

            //Jumping logic (Grace Periods Enabled)
            //Grounded
            if(player.state.grounded_grace){

                player.state.ticks_left_jumping = JUMP_DUR;

                //Standard-Jump
                if(starting_jump) {
                    player.state.jumping = true;
                    player.state.dy -= 1 * JUMP_STR;
                    player.state.grounded = false;
                    player.state.grounded_grace = 0;
                    player.state.pushing_wall = 0;
                    player.state.wall_grace = 0;
                    //HANDLING THIS ELSEWHERE
//                } else {
//                    player.state.grounded_grace--;
//                }
                }

            //Wall-Cling
            } if (player.state.wall_grace) {

                //Reset Jumps
                player.state.ticks_left_jumping = 2 * JUMP_DUR / 3;

                //Wall-Jump
                if (starting_jump ||                                                  //Enter Jump Command OR
                    (holding_jump && player.state.wall_grace * accel < 0)) {          //Hold Jump and move opposite dir

                    int dir = player.state.wall_grace < 0 ? 1 : -1;                   //Get that opposite dir
                    player.state.jumping = true;
                    player.state.pushing_wall = 0;
                    player.state.wall_grace = 0;
                    player.state.grounded = false;
                    player.state.grounded_grace = 0;
                    player.state.dy -= 1 * JUMP_STR * sin(M_PI / 3);
                    player.state.dx = dir * JUMP_STR * cos(M_PI / 3);
                    player.state.pos.x += dir * 2 * SIGMA; //Get off that wall
                    //HANDLING THIS ELSEWHERE
//                } else {
//                    player.state.wall_grace
//                }
                }
            }

//            printf("ground: %d\n", player.state.grounded);
//            printf("push: %d\n", player.state.pushing_wall);
//            printf("jumping: %d\n", player.state.jumping);
//            printf("-----------------\n");


            //Drag Calculations
            player.state.dx -= player.state.dx * DRAG_COEF;
            player.state.dy -= player.state.dy * DRAG_COEF;

            if (would_collide(player.state.dx, player.state.dy)){

                auto binary_search = [&](double dx, double dy) {
                    double low = 0.0;
                    double high = 1.0;
                    while(high - low > SIGMA/10){
                        double mid = low + (high - low)/2;
                        if (would_collide(mid * dx, mid * dy)) {
                            high = mid;
                        } else {
                            low = mid;
                        }
                    }

                    return Point { low * dx, low * dy };
                };

                Point motion = binary_search(player.state.dx, player.state.dy);

                if (motion.x == 0 && motion.y == 0) {
                    motion = binary_search(player.state.dx, 0);
                }

                if (motion.x == 0 && motion.y == 0) {
                    motion = binary_search(0, player.state.dy);
                }

                player.state.pos.x += motion.x;
                player.state.pos.y += motion.y;
            } else {
                player.state.pos.x += player.state.dx;
                player.state.pos.y += player.state.dy;
            }

            if (would_collide(0, SIGMA)){
                player.state.grounded = true;
                player.state.grounded_grace = GROUND_GRACE;
                player.state.dy = 0;
            } else {
                player.state.grounded = false;
                if (player.state.grounded_grace) player.state.grounded_grace--;
            }


            //Count the frames
            player.state.pushing_wall = 0;
            if (would_collide(SIGMA, 0)) {
                if(accel > 0) {
                    player.state.pushing_wall = 1;
                    player.state.wall_grace = WALL_GRACE;
                }
                player.state.dx = 0;
            } else if (would_collide(-SIGMA, 0) && accel < 0) {
                if(accel < 0){
                    player.state.pushing_wall = -1;
                    player.state.wall_grace = -WALL_GRACE;
                }
                player.state.dx = 0;
            } else if (player.state.wall_grace) {
                player.state.wall_grace -= player.state.wall_grace > 0 ? 1 : -1;
            }

            //Bullet logic
            if (player.state.ticks_till_next_bullet > 0) {
                player.state.ticks_till_next_bullet --;
            } else if (pull_trigger || holding_trigger) {
                player.state.ticks_till_next_bullet = player.state.gun->ticks_between_shots();

                std::vector<std::unique_ptr<Bullet>> next_bullets = player.spawn_bullets();

                for (auto& next_bullet: next_bullets) {
                    next_bullet->player_owner = i;

                    bullets.push_back(std::move(next_bullet));
                }

                if (player.state.ammo_left != -1) {
                    player.state.ammo_left --;

                    if (player.state.ammo_left <= 0) {
                        player.set_gun(create_gun("pistol"));
                    }
                }
            }
        }
    }

    std::vector<std::unique_ptr<Bullet>> next_bullets;


    std::vector<Rectangle> player_bounding_boxes;

    for (const auto& player: players) {
        if (!player.state.is_dead) {
            player_bounding_boxes.push_back(player.state.pos);
        }
    }

    auto damage_player_func = [&](int player_index, double damage) {
        damage_player(player_index, damage);
    };

    bool hit_something = false;

    // Update all the bullets
    for (auto& bullet : bullets) {
        double dx = bullet->get_velocity() * cos(bullet->angle);
        double dy = bullet->get_velocity() * sin(bullet->angle);

        bool is_dead = false;

        if (would_hit_ground(bullet->pos.offset(dx, dy))) {
            // This bullet hit the ground.
            hit_something = true;

            bool vertical_free = !would_hit_ground(bullet->pos.offset(0, dy));
            bool horizontal_free = !would_hit_ground(bullet->pos.offset(dx, 0));
            is_dead = bullet->on_wall_collision(player_bounding_boxes, damage_player_func, vertical_free, horizontal_free);
        }


        bullet->pos.x += dx;
        bullet->pos.y += dy;

        for (unsigned int i = 0; i < players.size(); i++) {
            auto& player = players[i];

            if (player.state.is_dead) {
                continue;
            }

            if (i == bullet->player_owner) {
                continue; // Don't intersect with self
            }

            if (bullet->pos.colliding_with(player.state.pos)) {
                hit_something = true;
                is_dead = bullet->on_player_collision(i, player_bounding_boxes, damage_player_func);
            }
        }

        for (unsigned int i = 0; i < boxes.size(); i++) {
            WeaponBox& box = boxes[i];

            if (!box.opened && bullet->pos.colliding_with(box.pos)) {
                is_dead = true;
                hit_something = true;
                box.opened = true;
            }
        }

        bool left_map = (bullet->pos.x < 0 || bullet->pos.x > level.width || bullet->pos.y < 0 || bullet->pos.y > level.height);

        if (!hit_something) {
            is_dead = bullet->on_no_collision();
        }

        if (!is_dead && !left_map) {
            next_bullets.push_back(std::move(bullet));
        } else {
            if (bullet->create_explosion_after_destruction()) {
                explosions.push_back(Explosion(bullet->pos.x, bullet->pos.y));
            }
        }

    }

    for (auto& box: boxes) {
        Rectangle new_pos = box.pos.offset(0, 1);
        if (!would_hit_ground(new_pos)) {
            box.pos = new_pos;;
        }
    }

    bullets = std::move(next_bullets);

    ticks_till_next_box--;

    if (ticks_till_next_box <= 0) {
        ticks_till_next_box = 600;
        for (int i = 0; i < 10; i++) {
            // Only try to spawn a box a maximum of 10 times ...
            WeaponBox possible_box = level.get_random_box_spawn(gen);

            if (!would_collide_or_fall_on_any(possible_box.pos)) {
                boxes.push_back(possible_box);
                break;
            }
        }
    }

    return nullptr;
}

void GameScreen::damage_player(int player_index, double damage) {
    if (game_over) {
        return;
    }

    auto& player = players[player_index];

    if (player.state.invincibility_ticks_left > 0) {
        return;
    }

    player.state.health -= damage;

    if (player.state.health <= 0 && !player.state.is_dead) {
        player.state.lives_left--;
        player.state.is_dead = true;

        if (player.state.lives_left == 0) {

            int num_alive = 0;
            for (const auto& other_player: players) {
                if (other_player.state.lives_left > 0) {
                    num_alive ++;
                }
            }

            if (num_alive <= 1) {
                // num alive can be 0 when we have a tie. Sometimes. It's a risk
                game_over = true;
            }
        } else {
            player.state.ticks_until_spawn = 130;
        }
    }
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

std::unique_ptr<Gun> GameScreen::attempt_pick_up(const Rectangle& rect) {
    std::vector<WeaponBox> next_boxes;

    std::unique_ptr<Gun> result = nullptr;

    for (const auto& box: boxes) {
        if (box.opened && box.pos.colliding_with(rect)) {
            result = create_gun(box.weapon);
        } else {
            next_boxes.push_back(box);
        }
    }

    boxes = next_boxes;

    return result;
}

