//
// Created by Chris Brown on 1/13/17.
//

#include "physicsengine.h"

void PhysicsEngine::update(State s, const std::map<int, inputs>& all_joystick_inputs,
                           const std::map<int, inputs>& all_last_inputs, SoundThread sounds){


    update_explosions(s);

    //TODO: refactor Pierce Effects

    update_players(s, all_joystick_inputs, all_last_inputs);

    // Update all the bullets
    for (auto& bullet : s.bullets) {

        bool hit_something = false;

        double dx = bullet->get_velocity() * cos(bullet->angle);
        double dy = bullet->get_velocity() * sin(bullet->angle);

        auto damage_player_func = [&](int player_index, double damage) {
            damage_player(s, player_index, damage, bullet->player_owner);
        };

        bool is_dead = false;

        if (would_hit_ground(bullet->pos.offset(dx, dy))) {
            // This bullet hit the ground.
            hit_something = true;

            bool vertical_free = !would_hit_ground(bullet->pos.offset(0, dy));
            bool horizontal_free = !would_hit_ground(bullet->pos.offset(dx, 0));
            is_dead = bullet->on_wall_collision(player_bounding_boxes, damage_player_func, vertical_free, horizontal_free, sounds);

            if (bullet->pierce_special_effect()) {

                bool found = false;
                for (auto& an_effect: pierce_effects) {
                    if (an_effect.bullet_id == bullet->id && !an_effect.has_end_yet) {
                        found = true;
                    }
                }

                if (!found) {
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


                    PierceEffectData new_effect;
                    new_effect.bullet_id = bullet->id;
                    new_effect.start = bullet->pos.offset(low * dx, low * dy).location();
                    new_effect.has_end_yet = false;
                    pierce_effects.push_back(new_effect);
                }
            }
        } else {
            if (bullet->pierce_special_effect()) {
                for (auto& an_effect: pierce_effects) {
                    if (an_effect.bullet_id == bullet->id && !an_effect.has_end_yet) {
                        an_effect.has_end_yet = true;
                        an_effect.ticks_left = 20;

                        double low = 0.0;
                        double high = 1.0;
                        while(high - low > SIGMA/10) {
                            double mid = low + (high - low)/2;
                            if (!would_hit_ground(bullet->pos.offset(mid * dx, mid * dy))) {
                                high = mid;
                            } else {
                                low = mid;
                            }
                        }

                        an_effect.end = bullet->pos.offset(low * dx, low * dy).location();
                    }
                }
            }
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
                if (player.state.invincibility_ticks_left == 0) {
                    std::uniform_real_distribution<> number(0, 1);
                    if (bullet->catch_on_fire() && number(gen) < FIRE_TRANSMIT_PROBABILITY) {
                        player.state.source_fire_player = bullet->player_owner;
                        player.state.ticks_fire_left = FIRE_LENGTH;
                    }
                }
                is_dead = bullet->on_player_collision(i, player_bounding_boxes, damage_player_func);

                if (is_dead && bullet->pierce_special_effect()) {
                    for (auto& an_effect: pierce_effects) {
                        if (an_effect.bullet_id == bullet->id && !an_effect.has_end_yet) {
                            an_effect.has_end_yet = true;
                            an_effect.ticks_left = 20;

                            double low = 0.0;
                            double high = 1.0;
                            while(high - low > SIGMA/10) {
                                double mid = low + (high - low)/2;
                                if (!would_hit_ground(bullet->pos.offset(mid * dx, mid * dy))) {
                                    high = mid;
                                } else {
                                    low = mid;
                                }
                            }

                            an_effect.end = bullet->pos.offset(low * dx, low * dy).location();
                        }
                    }
                }
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

        bool left_map = (bullet->pos.x < 0 || bullet->pos.x > level.width || bullet->pos.y < 0 || bullet->pos.y > level.height);

        if (!hit_something) {
            is_dead = bullet->on_no_collision();
        }

        if (!is_dead && !left_map) {
            next_bullets.push_back(std::move(bullet));
        } else {
            if (bullet->create_explosion_after_destruction()) {
                sounds.play_sound("../assets/sound/explosion.wav");
                explosions.push_back(Explosion(bullet->pos.x, bullet->pos.y));
            }

            if (bullet->create_little_explosion_after_destruction()) {
                explosions.push_back(Explosion(bullet->pos.x, bullet->pos.y, true));
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

void PhysicsEngine::update_players(State s, const std::map<int, inputs>& all_joystick_inputs, const std::map<int, inputs>& all_last_inputs){
    for (unsigned int i = 0; i < s.players.size(); i++) {
        auto &player = s.players[i];

        if (!player.state.is_dead){
            player.update();
            double accel = 0;

            if (player.state.ticks_fire_left > 0) {
                damage_player(s, i, FIRE_DMG_PER_TICK, player.state.source_fire_player);

                player.state.ticks_fire_left--;
                if (player.state.ticks_till_next_flame_particle == 0) {
                    std::uniform_real_distribution<> dist(-10, 10);

                    auto next_bullet = std::make_unique<FlameBullet>();

                    next_bullet->ticks_left = 20 + dist(gen);

                    next_bullet->pos.x = player.state.pos.x + dist(gen);
                    next_bullet->pos.y = player.state.pos.y + dist(gen) * 3;
                    next_bullet->angle = -M_PI / 2 + dist(gen)/20;
                    next_bullet->player_owner = i;
                    s.bullets.push_back(std::move(next_bullet));

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

                std::vector<std::unique_ptr<Bullet>> next_bullets = player.spawn_bullets();

                for (auto& next_bullet: next_bullets) {
                    next_bullet->player_owner = i;

                    s.bullets.push_back(std::move(next_bullet));
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
}

void PhysicsEngine::update_explosions(State s){
    std::vector<Explosion> next_explosions;

    for (auto& explosion: s.explosions) {
        explosion.update();

        if (!explosion.is_done()) {
            next_explosions.push_back(explosion);
        }
    }

    s.explosions = next_explosions;
}

void PhysicsEngine::damage_player(State s, int player_index, double damage, int shooter_index) {
    const int DEATH_PENALTY = 1;
    const int SUICIDE_PENALTY = 2;
    const int KILL_VALUE = 2;
    auto& player = s.players[player_index];

    if (player.state.invincibility_ticks_left > 0) {
        return;
    }

    player.state.health -= damage;

    if (player.state.health <= 0 && !player.state.is_dead) {
        player.state.is_dead = true;

        auto& shooter = s.players[shooter_index];

        if (shooter_index != player_index) {
            shooter.state.score += KILL_VALUE;
        } else {
            shooter.state.score = std::max(0, player.state.score - SUICIDE_PENALTY);
        }

        player.state.score = std::max(0, player.state.score - DEATH_PENALTY);
        player.state.ticks_until_spawn = 130;

    }
}

