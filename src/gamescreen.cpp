#include "gamescreen.h"

#include "gameoverscreen.h"

#include <string>
#include <cmath>

#define DRAG_COEF 0.04
#define JUMP_STR 0.3
#define GRAVITY 0.04
#define X_ACCEL 0.02
#define BULLET_VEL 0.3
#define JUMP_DUR 12
#define PLAYER_WIDTH 0.5
#define PLAYER_HEIGHT 0.5

GameScreen::GameScreen(const std::shared_ptr<std::vector<Player>> &play){

    players = play;

	// Initialize the player state
	for (unsigned int i = 0; i < play->size(); i++) {
        auto& player = (*play)[i];

		switch (i) {
			case 0:
				player.setPosition(1.25, 8.5);
				break;

			case 1:
				player.setPosition(18.75, 8.5);
				break;
		}
	}

    // Initialize the level
        std::vector<std::vector<Terrain>> grid;// = new std::vector<std::vector<Terrain>>;

        // Set up sizes. (HEIGHT x WIDTH)
        grid.resize(20);
        for (int i = 0; i < 20; ++i)
            grid[i].resize(10);

    for (int x = 0; x < 20; x++) {
        for (int y = 0; y < 10; y++) {

            if (x == 0 || x == 19 || y == 0 || y == 9) {
                // Add borders around the map
                grid[x][y] = Terrain ::GROUND;
            } else if (x >=3 && x <= 5 && y == 7) {
                // Add a little platform
                grid[x][y] = Terrain ::GROUND;
            } else if (x >= 14 && x <= 16 && y == 7) {
                // Add a little platform
                grid[x][y] = Terrain ::GROUND;
            } else if (x >= 7 && x <= 12 && y == 5){
                // Add a little platform
                grid[x][y] = Terrain ::GROUND;
            } else {
                grid[x][y] = Terrain ::OPEN;
            }

        }
    }
    level = Level(20, 10, grid); //TODO is there a better way to handle this, should this just be a vector?

}

void GameScreen::render(RenderList& list, double mouseX, double mouseY) {

	level.draw(list);

	// Render the players.
	for (auto& player : *players) {
		const char* color = nullptr;

		switch (player.getAttributes().color) {
			case PlayerColor::RED:
				color = "stickRed";
				break;

			case PlayerColor::BLUE:
				color = "stickBlue";
				break;

			case PlayerColor::YELLOW:
				color = "stickYellow";
				break;

			case PlayerColor::GREEN:
				color = "stickGreen";
				break;
		}

        axes pos = player.getState().position;

		list.translate(pos.x * 60, pos.y * 60);
		list.add_image(color, -15, -30, 30, 60);

		// Render the health bar
		list.add_image("black", -15, -36, 30, 8);
		list.add_image("red", -13, -34, 26, 4);
		list.add_image("green", -13, -34, 26 * player.getState().health / 100.0, 4);
		
		// Render the gun

		if (!player.getState().facing_right) {
			list.scale(-1, 1);
		}

        double gun_angle = player.getState().facing_right ? player.getState().gun_angle : M_PI - player.getState().gun_angle;

		list.translate(-15 + 26, -30 + 35);
		list.rotate(gun_angle);
		list.add_image("simpleGun", -2, -5);
		list.rotate(-gun_angle);
		list.translate(-(-15 + 26), -(-30 + 35));

		if (!player.getState().facing_right) {
			list.scale(-1, 1);
		}

		list.translate(-pos.x * 60, -pos.y * 60);
	}

	// Render the bullets
	for (const auto& bullet: bullets) {
		list.add_image("bullet", bullet.x * 60 - 5, bullet.y * 60 - 5);
	}
}

std::unique_ptr<Screen> GameScreen::update(GLFWwindow* window) {


	for (unsigned int i = 0; i < players->size(); i++) {
        auto &player = (*players)[i];

        bool firing_bullet = false;
        bool attempting_jump = false;

        double accel = 0;

        PlayerState s = player.getState();

        // Deal with player input.
        inputs player_inputs = player.getInputs();

        //Threshold the motion so that you aren't creeping around at slow speed
        float x_thresh = (fabs(player_inputs.ls.x) > 0.3) ? player_inputs.ls.x : 0;
        accel += x_thresh * X_ACCEL;

        //Directional aiming, but if player isn't aiming, aim flat in direction of motion.
        if (fabs(player_inputs.rs.x) < 0.3 && fabs(player_inputs.rs.y) < 0.3) {

            //Original code
            //This line is a travesty and I'm happy about that (direction of motion aim, keep direction if not moving)
            int right = (s.velocity.x > 0) ? 1 : (s.velocity.x < 0) ? 0 : (s.facing_right) ? 1 : 0;
            s.gun_angle = M_PI - right * M_PI;

            //This code changes it to the left stick controlling aim in the absence of right stick controls
            //Still reverts to flat aim in previous direction if no input is provided
//                if (fabs(player_inputs.ls.x) < 0.3 && fabs(player_inputs.ls.y) < 0.3) {
//                    player.gun_angle = player.facing_right ? 0 : M_PI;
//                } else {
//                    player.gun_angle = atan2(player_inputs.ls.y, player_inputs.ls.x);
//                }

        } else {
            s.gun_angle = atan2(player_inputs.rs.y, player_inputs.rs.x);
        }

        //Checking inputs for later calculations
        attempting_jump = player_inputs.rb || player_inputs.lt || player_inputs.a; //TODO determine best input
        firing_bullet = player_inputs.rt;

        //Direction is based on way you're aiming if you're aiming
        if (fabs(s.gun_angle) < M_PI / 2) {
            s.facing_right = true;
        } else {
            s.facing_right = false;
        }

        //X-Speed considerations for stopping and turning around on the ground
        if (is_colliding_with_ground(s.position.x, s.position.y + 0.1, 0.5 - 0.1, 1 - 0.1)) {\
            if (accel == 0) {
                s.velocity.x = s.velocity.x / 4;
                if (fabs(s.velocity.x) < 0.2 * X_ACCEL) {
                    s.velocity.x = 0;
                }
            } else if (fabs(s.velocity.x + accel) < fabs(s.velocity.x)) {
                s.velocity.x = s.velocity.x = (s.velocity.x / 4) + accel;
            } else {
                s.velocity.x += accel;
            }
        } else {
            //X-Speed considerations for aerial motion
            s.velocity.x += accel;
        }

        //Speed cap for horizontal motion
        //TODO figure out if we want to alter or remove the cap when not grounded
        //if (fabs(player.dx) > MAX_X_SPEED){
        //    player.dx = (player.dx > 0) ? MAX_X_SPEED : -MAX_X_SPEED;
        //}

        //Jumping/Gravity logic
        //Grounded
        if(is_colliding_with_ground(s.position.x, s.position.y + 0.1, 0.5 - 0.1, 1 - 0.1)){

            //Reset jumps
            s.is_jumping = false;
            s.velocity.y = 0;

            //Standard-Jump
            if(attempting_jump){
                s.is_jumping = true;
                s.velocity.y -= 1 * JUMP_STR;
                s.ticks_left_jumping = JUMP_DUR;
            }

        //Wall-Cling
            // TODO: Fix the jank here
//        } else if (player.touching_wall){ -- we need a way to represent direction, this might be 1 or -1 to indicate that
//
//            //Reset jumps
//            player.double_jump = false;
//            player.is_jumping = false;
//
//            //Half speed fall with friction
//            //TODO revisit formula
//            player.dy = (player.dy + GRAVITY)/2;
//
//            //Wall-Jump
//            if(attempting_jump){
//                int dir = accel > 0 ? -1 : 1;
//                player.is_jumping = true;
//                player.dy -= 1 * JUMP_STR * cos(M_PI/2);
//                player.dx = dir * JUMP_STR * cos(M_PI/2);
//                player.ticks_left_jumping = JUMP_DUR;
//            }
//
//        //Aerial Logic
        } else {
            //Continued-Jump
            if(s.is_jumping){
                if(attempting_jump){
                    if (s.ticks_left_jumping >= 0) {
                        s.ticks_left_jumping--;
                        //To smoothly counteract the falling motion
                        s.velocity.y -= GRAVITY; // * (player.ticks_left_jumping/(double)JUMP_DIR);
                    }
                } else {
                    s.is_jumping = false;
                    s.ticks_left_jumping = 0;
                    s.velocity.y = 0;
                }
            }
            //Gravity always has an effect in the air.
            s.velocity.y += GRAVITY;
            //if (fabs(player.dy) > MAX_Y_SPEED){
            //    player.dy = (player.dy > 0) ? MAX_Y_SPEED : -MAX_Y_SPEED;
            //}
        }

        //Drag Calculations
        s.velocity.x -= s.velocity.x * DRAG_COEF;
        s.velocity.y -= s.velocity.y * DRAG_COEF;

        //Collision tracking super basic

        double low = 0.0;
        double mid = 0.5;
        double high = 1.0;


        if(is_colliding_with_ground(s.position.x + s.velocity.x, s.position.y + s.velocity.y, 0.5 - 0.1, 1 - 0.1)){
            while(high - low > 0.005){
                if(is_colliding_with_ground(s.position.x + mid * s.velocity.x, s.position.y + mid * s.velocity.y, 0.5 - 0.1, 1-0.1)){
                    high = mid;
                } else {
                    low = mid;
                }
                mid = low + (high - low)/2;
            }
            //TODO logic that slides along colliding surface
            s.position.x += mid * s.velocity.x;
            s.position.y += mid * s.velocity.y;
        } else {
            s.position.x += s.velocity.x;
            s.position.y += s.velocity.y;
        }

        //Make motions as necessary


        //Bullet logic
        if (s.ticks_till_next_bullet > 0) {
            s.ticks_till_next_bullet --;
        } else if (firing_bullet) {
            s.ticks_till_next_bullet = 15;

            Bullet next_bullet;
            next_bullet.player_owner = i;

            double scale_factor = s.facing_right ? 1 : -1;
            next_bullet.x = s.position.x + scale_factor * ((-15 + 26)/60.0) + 15.0/60 * cos(-17.0 * M_PI / 180.0 + s.gun_angle);
            next_bullet.y = s.position.y + (-30 + 35)/60.0 + 15.0/60 * sin(-17.0 * M_PI / 180.0 + s.gun_angle);

            next_bullet.x_vel = BULLET_VEL * cos(s.gun_angle);
            next_bullet.y_vel = BULLET_VEL * sin(s.gun_angle);

            bullets.push_back(next_bullet);
        }

        player.updateState(s);

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

		for (unsigned int i = 0; i < players->size(); i++) {
			auto& player = (*players)[i];

			if (i == bullet.player_owner) {
				continue; // Don't intersect with self
			}

            PlayerState s = player.getState();

			if (rect_rect_colliding(bullet.x, bullet.y, 10/60.0, 10/60.0, s.position.x, s.position.y, 0.5, 1)) {
				hit_a_player = true;
				s.health -= 10;

				if (s.health == 0) {
					auto& otherPlayer = (*players)[1 - i];
					return std::make_unique<GameOverScreen>(otherPlayer.getAttributes());
				}

                player.updateState(s);
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