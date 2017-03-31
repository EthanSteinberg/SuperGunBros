#include "gamescreen.h"

#include "readyscreen.h"
#include <string>
#include <cmath>

#include "soundthread.h"

#define DRAG_COEF 0.075
#define JUMP_STR 15
#define GRAVITY 0.75
#define X_ACCEL 0.5
#define JUMP_DUR 10
#define SIGMA 0.001
#define BOOST_STR 1
#define BOOST_DUR 180
#define WALL_FRICTION 0.2
#define WALL_GRACE 5
#define GROUND_GRACE 5

const int SCORE_TO_WIN = 10;

const int FIRE_LENGTH = 200;

const double FIRE_TRANSMIT_PROBABILITY = 0.01;

GameScreen::GameScreen(const std::vector<PlayerInfo> &infos, const Level& a_level): level(a_level), camera(level.width, level.height) {

    // Initialize the player state
    for (unsigned int i = 0; i < infos.size(); i++) {
        const auto& info = infos[i];
        Point p = level.get_player_spawn_locations()[(int)info.color];

        Player player(p.x, p.y, info);
        players.push_back(std::move(player));
    }

    for (const auto& box_spawn : level.get_box_spawns()) {
        boxes.push_back(box_spawn.get_random_spawn(gen, true));
    }
}

void GameScreen::render(RenderList& list) const {

    list.push();
    camera.transform(list);

    level.render(list);

    for (const auto& explosion: explosions) {
        explosion.render(list);
    }

    // Render the weapon boxes
    for (const auto& box: boxes) {
        box.render(list);
    }

    int max_score = -1;
    std::vector<int> players_with_max_score;

    for (unsigned int i = 0; i < players.size(); i++) {
        const auto& player = players[i];

        if (player.state.score == max_score) {
            players_with_max_score.push_back(i);
        } else if (player.state.score > max_score) {
            players_with_max_score.clear();
            players_with_max_score.push_back(i);
            max_score = player.state.score;
        }
    }

	// Render the players.

	for (const auto& player : players) {
        player.render(list);

        if (players_with_max_score.size() != players.size()) {
            if (player.state.score == max_score) {
                player.render_crown(list);
            }
        }
	}

	// Render the bullets
	for (const auto& bullet: bullets) {
        bullet->render(list);
	}

    for (const auto& effect: pierce_effects) {
        list.add_line("white", effect.start.x, effect.start.y, effect.end.x, effect.end.y);
    }

    list.pop();

    list.push();
    list.set_z(100);

    Rectangle score_board_rect = list.get_image_dimensions("score-board");
    score_board_rect = score_board_rect.offset(1280/2, score_board_rect.height/2);
    list.add_rect("score-board", score_board_rect);

    for (const auto& player : players) {
        int x = 0;
        int y = 0;
        const int top_row_offset = 129;
        const int top_row_y = 28;

        const int bottom_row_offset = 100;
        const int bottom_row_y = 65;

        switch (player.info.color) {
            case PlayerColor::RED:
                x = 1280/2 - top_row_offset;
                y = top_row_y;
                break;

            case PlayerColor::BLUE:
                x = 1280/2 + top_row_offset;
                y = top_row_y;
                break;

            case PlayerColor::YELLOW:
                x = 1280/2 - bottom_row_offset;
                y = bottom_row_y;
                break;

            case PlayerColor::GREEN:
                x = 1280/2 + bottom_row_offset;
                y = bottom_row_y;
                break;

            default:
                std::cout<<"Invalid color " << (int)player.info.color << std::endl;
                exit(-1);
        }

        if (player.state.ticks_since_last_score_update != -1 &&
            player.state.ticks_since_last_score_update < 120) {
            int phase = player.state.ticks_since_last_score_update/24;
            if (phase % 2 == 0) {
                list.add_image(get_color_name(player.info.color), x - 25, y - 18, 50, 36);
            }

        }

        list.add_red_numbers(get_color_name(player.info.color), player.state.score, x, y);
    }

    if (game_over) {
        std::string winning_color = "";

        for (const auto& player: players) {
            if (player.state.score >= SCORE_TO_WIN) {
                if (winning_color == "") {
                    winning_color = get_color_name(player.info.color);
                } else {
                    winning_color = "tie";
                }
            }
        }

        list.add_image("player-won-" + winning_color, (1280 - 1131)/2, 100);

        list.add_image("press-start-continue", (1280 - 1112)/2, 500);
    }

    list.pop();
}

std::unique_ptr<Screen> GameScreen::update(const std::map<int, inputs>& all_joystick_inputs, const std::map<int, inputs>& all_last_inputs, SoundThread& sounds) {

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

    std::vector<PierceEffectData> next_pierce_effects;
    for (auto& effect: pierce_effects) {
        if (effect.ticks_left-- > 0) {
            next_pierce_effects.push_back(effect);
        }
    }

    pierce_effects = next_pierce_effects;

    for (unsigned int i = 0; i < players.size(); i++) {
        auto &player = players[i];

        if (player.state.ticks_since_last_score_update != -1) {
            player.state.ticks_since_last_score_update += 1;
        }

        if (player.state.is_dead) {

            if (player.state.current_player_shooting_sound_filename != nullptr) {
                player.state.current_player_shooting_sound_filename = nullptr;
                sounds.stop_sound(player.state.current_player_shooting_sound);
            }

            player.state.ticks_until_spawn --;

            if (player.state.ticks_until_spawn == 0) {
                player.state.is_dead = false;

                player.state.health = MAX_HEALTH;

                Point spawn_location = level.get_random_player_spawn_location(gen);

                player.state.pos.x = spawn_location.x;
                player.state.pos.y = spawn_location.y;

                player.state.ticks_fire_left = 0;

                player.state.invincibility_ticks_left = DEATH_TIME;
            }
        } else {
            player.update();
            double accel = 0;

            if (level.in_killbox(player.state.pos.x, player.state.pos.y)){
                damage_player(i, MAX_HEALTH, player.state.ticks_fire_left > 0 ? player.state.source_fire_player : i);
            }

            if (player.state.ticks_fire_left > 0) {
                damage_player(i, FIRE_DMG_PER_TICK, player.state.source_fire_player);

                player.state.ticks_fire_left--;
                if (player.state.ticks_till_next_flame_particle == 0) {
                    std::uniform_real_distribution<> dist(-10, 10);

                    auto next_bullet = std::make_unique<FlameBullet>(true);

                    next_bullet->ticks_left = 20 + dist(gen);

                    next_bullet->pos.x = player.state.pos.x + dist(gen);
                    next_bullet->pos.y = player.state.pos.y + dist(gen) * 3;
                    next_bullet->angle = -M_PI / 2 + dist(gen)/20;
                    next_bullet->player_owner = i;
                    bullets.push_back(std::move(next_bullet));

                    player.state.ticks_till_next_flame_particle = 3;
                } else {
                    player.state.ticks_till_next_flame_particle --;
                }
            }

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

            if (player.state.sticky_shooting) {
                const int num_stops = 16;
                double rads_per_stop = (2 * M_PI / num_stops);

                double index = std::round(player.state.gun_angle / rads_per_stop);

                player.state.gun_angle = rads_per_stop * index;
            } else {
                if (std::abs(player.state.gun_angle) < 10 * M_PI/180.0) {
                    player.state.gun_angle = 0;
                } else if ((M_PI - std::abs(player.state.gun_angle) < 10 * M_PI/180.0)) {
                    player.state.gun_angle = M_PI;
                }
            }

            if (button_press(ButtonName::R3, current_inputs, prev_inputs)) {
                player.state.sticky_shooting = !player.state.sticky_shooting;
            }

            //Checking inputs for later calculations
            bool starting_jump = button_press(ButtonName::LT, current_inputs, prev_inputs) ||
                                 button_press(ButtonName::A, current_inputs, prev_inputs);
            bool holding_jump = button_hold(ButtonName::LT, current_inputs, prev_inputs) ||
                                button_hold(ButtonName::A, current_inputs, prev_inputs);

            bool pull_trigger = button_press(ButtonName::RT, current_inputs, prev_inputs);
            bool holding_trigger = button_hold(ButtonName::RT, current_inputs, prev_inputs);

            bool starting_boost = button_press(ButtonName::LB, current_inputs, prev_inputs) ||
                                    button_press(ButtonName::B, current_inputs, prev_inputs);
            bool continuing_boost = (button_hold(ButtonName::LB, current_inputs, prev_inputs) ||
                                    button_hold(ButtonName::B, current_inputs, prev_inputs)) &&
                                    player.state.boosting;

            for (auto& box: boxes) {
                if (box.ticks_until_active != 0) {
                    continue;
                }

                if (box.pos.colliding_with(player.state.pos)) {
                    box.opened = true;
                }
            }

            bool attempting_pickup = button_press(ButtonName::X, current_inputs, prev_inputs);

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
            } else if (player.state.jumping) {
                // Jumping logic
                if (would_collide(0, -2 * GRAVITY)) {
                    // Would hit the ceiling. Stop jumping.
                    player.state.jumping = false;
                    player.state.ticks_left_jumping = 0;
                    player.state.dy += GRAVITY;
                } else if (holding_jump && player.state.ticks_left_jumping > 0){
                    player.state.ticks_left_jumping--;
                    //To smoothly counteract the falling motion
                    player.state.dy -= GRAVITY; // * (player.ticks_left_jumping/(double)JUMP_DIR);

                } else {
                    player.state.jumping = false;
                    player.state.ticks_left_jumping = 0;
                }
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
                    player.state.ticks_left_jumping = 2 * JUMP_DUR / 3;
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
                    motion = binary_search(0, player.state.dy);
                }

                if (motion.x == 0 && motion.y == 0) {
                    // We can't go up, we can't continue jumping.
                    motion = binary_search(player.state.dx, 0);
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

                if (player.state.gun->shoot_sound() != nullptr) {
                    sounds.play_sound(player.state.gun->shoot_sound());
                }

                player.state.ticks_till_next_bullet = player.state.gun->ticks_between_shots();
                std::vector<std::unique_ptr<Bullet>> new_bullets = player.spawn_bullets();


                for (auto& next_bullet: new_bullets) {

                    if (level.intersects_line(player.state.pos.x, player.state.pos.y, next_bullet->pos.x, next_bullet->pos.y)) {
                        double dx = next_bullet->pos.x - player.state.pos.x;
                        double dy = next_bullet->pos.y - player.state.pos.y;

                        double low = 0.0;
                        double high = 1.0;
                        while(high - low > SIGMA/10){
                            double mid = low + (high - low)/2;
                            if (level.intersects_line(player.state.pos.x, player.state.pos.y, player.state.pos.x + mid * dx, player.state.pos.y + mid * dy)) {
                                high = mid;
                            } else {
                                low = mid;
                            }
                        }

                        next_bullet->pos.x = player.state.pos.x + low * dx;
                        next_bullet->pos.y = player.state.pos.y + low * dy;
                    }

                    next_bullet->player_owner = i;

                    if (next_bullet->pierce_special_effect()) {
                        // If it is a piercing bullet, we have to do a hitscan sort of attack

                        double dx = cos(next_bullet->angle);
                        double dy = sin(next_bullet->angle);
                        double max_time = level.get_first_non_pierce_intersection(next_bullet->pos.x, next_bullet->pos.y, dx, dy);

                        Point start = next_bullet->pos.location();
                        Point end = next_bullet->pos.offset(dx * max_time, dy * max_time).location();

                        for (unsigned int other_i = 0; other_i < players.size(); other_i++) {
                            const auto& player = players[other_i];
                            if (other_i == next_bullet->player_owner) {
                                continue;
                            }

                            if (player.state.pos.intersects_line(start.x, start.y, end.x, end.y)) {
                                damage_player(other_i, 10, i);
                            }
                        }
                        for (auto& box: boxes) {
                            if (box.ticks_until_active != 0) {
                                continue;
                            }

                            if (box.pos.intersects_line(start.x, start.y, end.x, end.y)) {
                                box.opened = true;
                            }
                        }

                        pierce_effects.push_back(PierceEffectData{start, end, 10});
                    } else {
                       bullets.push_back(std::move(next_bullet));
                   }

                }

                if (player.state.ammo_left != -1) {
                    player.state.ammo_left --;

                    if (player.state.ammo_left <= 0) {
                        player.set_gun(create_gun("pistol"));
                    }
                }
            }

            const char* desired_shooting_sound = nullptr;

            if (pull_trigger || holding_trigger) {
                desired_shooting_sound = player.state.gun->holding_shoot_sound();
            }

            if (desired_shooting_sound != player.state.current_player_shooting_sound_filename) {
                if (player.state.current_player_shooting_sound_filename != nullptr) {
                    sounds.stop_sound(player.state.current_player_shooting_sound);
                }

                if (desired_shooting_sound != nullptr) {
                    player.state.current_player_shooting_sound = sounds.play_sound(desired_shooting_sound, true);
                }

                player.state.current_player_shooting_sound_filename = desired_shooting_sound;
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

    // Update all the bullets
    for (auto& bullet : bullets) {

        bool hit_something = false;

        double dx = bullet->get_velocity() * cos(bullet->angle);
        double dy = bullet->get_velocity() * sin(bullet->angle);

        auto damage_player_func = [&](int player_index, double damage) {
            damage_player(player_index, damage, bullet->player_owner);
        };

        bool is_dead = false;

        bool hit_blocker = false;

        if (!bullet->can_block()) {
            for (auto& other_bullet: bullets) {
                if (other_bullet == nullptr) continue;
                if (other_bullet->can_block() && other_bullet->pos.colliding_with(bullet->pos.offset(dx, dy))) {
                    hit_blocker = true;
                    break;
                }
            }
            for (auto& other_bullet: next_bullets) {
                if (other_bullet == nullptr) continue;
                if (other_bullet->can_block() && other_bullet->pos.colliding_with(bullet->pos.offset(dx, dy))) {
                    hit_blocker = true;
                    break;
                }
            }
        }

        if (hit_blocker) {
            hit_something = true;
            is_dead = bullet->on_blocker_collision(player_bounding_boxes, damage_player_func);
        } else if (would_hit_ground(bullet->pos.offset(dx, dy))) {
            // This bullet hit the ground.
            hit_something = true;

            double low = 0.0;
            double high = 1.0;
            while(high - low > SIGMA/10){
                double mid = low + (high - low)/2;
                if (would_hit_ground(bullet->pos.offset(mid * dx, mid * dy))) {
                    high = mid;
                } else {
                    low = mid;
                }
            }

            bool vertical_free = !would_hit_ground(bullet->pos.offset(0, dy));
            bool horizontal_free = !would_hit_ground(bullet->pos.offset(dx, 0));

            is_dead = bullet->on_wall_collision(player_bounding_boxes, damage_player_func, vertical_free, horizontal_free, sounds);


            dx = high * dx;
            dy = high * dy;
        }

        bullet->pos.x += dx;
        bullet->pos.y += dy;

        for (unsigned int i = 0; i < players.size(); i++) {
            auto& player = players[i];

            if (player.state.is_dead) {
                continue;
            }

            if (i == bullet->player_owner && !bullet->can_damage_self()) {
                continue;
            }

            if (bullet->pos.colliding_with(player.state.pos)) {
                hit_something = true;
                if (player.state.invincibility_ticks_left == 0) {
                    std::uniform_real_distribution<> number(0, 1);
                    if (bullet->catch_on_fire() && number(gen) < FIRE_TRANSMIT_PROBABILITY) {
                        player.state.source_fire_player = bullet->player_owner;
                        player.state.ticks_fire_left = FIRE_LENGTH;
                    }
                }
                is_dead = bullet->on_player_collision(i, player_bounding_boxes, damage_player_func);
            }
        }

        for (unsigned int i = 0; i < boxes.size(); i++) {
            WeaponBox& box = boxes[i];

            if (box.ticks_until_active == 0 && !box.opened && bullet->pos.colliding_with(box.pos)) {
                is_dead = true;
                hit_something = true;
                box.opened = true;
            }
        }

        //Slap a margin on that for good measure, it was looking weird on borderless maps
        int MAP_MARGIN = 500;
        bool left_map = (bullet->pos.x < 0 - MAP_MARGIN || bullet->pos.x > level.width + MAP_MARGIN || bullet->pos.y < 0 - MAP_MARGIN|| bullet->pos.y > level.height + MAP_MARGIN);

        if (!hit_something) {
            is_dead = bullet->on_no_collision();
        }

        if (!is_dead && !left_map) {
            next_bullets.push_back(std::move(bullet));
        } else {
            ExplosionType type = bullet->get_explosion();

            if (type != ExplosionType::NONE) {

                if (type == ExplosionType::ROCKET) {
                    sounds.play_sound("../assets/sound/explosion.wav");
                }

                explosions.push_back(Explosion(bullet->pos.x, bullet->pos.y, type));

            }
        }

    }

    bullets = std::move(next_bullets);

    for (auto& box: boxes) {
        Rectangle new_pos = box.pos.offset(0, 1);
        if (box.ticks_until_active > 0) {
            box.ticks_until_active--;
        }
        if (!would_hit_ground(new_pos)) {
            box.pos = new_pos;;
        }
    }

    return nullptr;
}

void GameScreen::damage_player(int player_index, double damage, int shooter_index) {
    if (game_over) {
        return;
    }

    auto& player = players[player_index];

    if (player.state.invincibility_ticks_left > 0) {
        return;
    }

    player.state.health -= damage;

    if (player.state.health <= 0 && !player.state.is_dead) {
        player.state.is_dead = true;

        //TODO: dropped weapons
//        WeaponBox b = WeaponBox()
//        boxes.push_back();

        player.set_gun(create_gun("pistol"));

        auto& shooter = players[shooter_index];

        if (shooter_index != player_index) {
            shooter.state.score += 2;
            shooter.state.ticks_since_last_score_update = 0;
        } else {
        }

        player.state.score = std::max(0, player.state.score - 1);

        if (shooter.state.score >= SCORE_TO_WIN) {
            game_over = true;
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

std::unique_ptr<Gun> GameScreen::attempt_pick_up(const Rectangle& rect) {
    std::unique_ptr<Gun> result = nullptr;

    for (auto& box: boxes) {
        if (box.ticks_until_active != 0) {
            continue;
        }

        if (box.pos.colliding_with(rect)) {
            result = std::move(box.weapon);
            box = level.get_box_spawns()[box.spawn_index].get_random_spawn(gen, false);
        }
    }

    return result;
}
