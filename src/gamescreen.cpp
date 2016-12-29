#include "gamescreen.h"

#include "gameoverscreen.h"

#include <string>
#include <cmath>

#define MAX_X_SPEED 0.1
#define MAX_Y_SPEED 0.2
#define JUMP_COEF 0.1
#define GRAVITY 0.05
#define X_ACCEL 0.01
//Affects acceleration in the air
#define AERIAL_ACCEL_COEF 0.5
#define BULLET_VEL 0.1
#define JUMP_DIR 20
#define SIGMA 0.001

GameScreen::GameScreen(const std::vector<PlayerInfo>& infos) {

	// Initialize the player state
	for (unsigned int i = 0; i < infos.size(); i++) {
		const auto& info = infos[i];
		PlayerState player = {
			0,8.5, 0, 0, false, info, 0, true, false, true, 0, 100
		};
		switch (i) {
			case 0:
				player.x = 1.25;
				break;

			case 1:
				player.x = 18.75;
				break;
		}
		players.push_back(player);
	}

	// Initialize the map
	for (int x = 0; x < 20; x++) {
		for (int y = 0; y < 10; y++) {

			if (x == 0 || x == 19 || y == 0 || y == 9) {
				// Add borders around the map
				is_ground[x][y] = true;
			} else if (x >=3 && x <= 5 && y == 7) {
                // Add a little platform
                is_ground[x][y] = true;
            } else if (x >= 14 && x <= 16 && y == 7) {
                // Add a little platform
                is_ground[x][y] = true;
            } else if (x >= 7 && x <= 12 && y == 5){
                // Add a little platform
                is_ground[x][y] = true;
			} else {
				is_ground[x][y] = false;
			}

		}
	}
}

void GameScreen::render(RenderList& list, double mouseX, double mouseY) {

	// Render the map
	for (int x = 0; x < 20; x++) {
		for (int y = 0; y < 10; y++) {

			if (is_ground[x][y]) {
				list.add_image("black", x * 60, y * 60, 60, 60);
			}
		}
	}

	// Render the players.
	for (const auto& player : players) {
		const char* color = nullptr;

		switch (player.info.color) {
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

		list.translate(player.x * 60, player.y * 60);
		list.add_image(color, -15, -30, 30, 60);

		// Render the health bar
		list.add_image("black", -15, -36, 30, 8);
		list.add_image("red", -13, -34, 26, 4);
		list.add_image("green", -13, -34, 26 * player.health / 100.0, 4);
		
		// Render the gun

		if (!player.facing_right) {
			list.scale(-1, 1);
		}

        double gun_angle = player.facing_right ? player.gun_angle : M_PI - player.gun_angle;

		list.translate(-15 + 26, -30 + 35);
		list.rotate(gun_angle);
		list.add_image("simpleGun", -2, -5);
		list.rotate(-gun_angle);
		list.translate(-(-15 + 26), -(-30 + 35));

		if (!player.facing_right) {
			list.scale(-1, 1);
		}

		list.translate(-player.x * 60, -player.y * 60);
	}

	// Render the bullets
	for (const auto& bullet: bullets) {
		list.add_image("bullet", bullet.x * 60 - 5, bullet.y * 60 - 5);
	}
}

std::unique_ptr<Screen> GameScreen::update(GLFWwindow* window) {
	for (unsigned int i = 0; i < players.size(); i++) {
        auto &player = players[i];

        bool firing_bullet = false;
        bool attempting_jump = false;

        double accel = 0;

        // Deal with player input.
        if (player.info.type == PlayerType::KEYBOARD) {

            //X-axis
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { //Changed from left arrow
                accel -= X_ACCEL;
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { //Changed from right arrow
                accel += X_ACCEL;
            }

            //Directional Aiming
            double cursorX;
            double cursorY;
            glfwGetCursorPos(window, &cursorX, &cursorY);
            double xdiff = cursorX - (player.x * 30);
            double ydiff = cursorY - (player.y * 30);
            player.gun_angle = atan2(ydiff, xdiff);

            //Gather inputs for later calculations
            firing_bullet = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS; //changed from space bar
            attempting_jump = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS; //Changed from up key

        } else if (player.info.type == PlayerType::GAMEPAD) {
            inputs player_inputs = player.info.gamePad->getState();

            //Threshold the motion so that you aren't creeping around at slow speed
            float x_thresh = (fabs(player_inputs.ls.x) > 0.3) ? player_inputs.ls.x : 0;
            accel += x_thresh * X_ACCEL;

            //Directional aiming, but if player isn't aiming, aim flat in direction of motion.
            if (fabs(player_inputs.rs.x) < 0.3 && fabs(player_inputs.rs.y) < 0.3) {

                //Original code
                //This line is a travesty and I'm happy about that (direction of motion aim, keep direction if not moving)
                //int right = (dx > 0) ? 1 : (dx < 0) ? 0 : (player.facing_right) ? 1 : 0;
                //player.gun_angle = M_PI - right * M_PI;

                //This code changes it to the left stick controlling aim in the absence of right stick controls
                //Still reverts to flat aim in previous direction if no input is provided
                if (fabs(player_inputs.ls.x) < 0.3 && fabs(player_inputs.ls.y) < 0.3) {
                    player.gun_angle = player.facing_right ? 0 : M_PI;
                } else {
                    player.gun_angle = atan2(player_inputs.ls.y, player_inputs.ls.x);
                }
            } else {
                player.gun_angle = atan2(player_inputs.rs.y, player_inputs.rs.x);
            }

            //Checking inputs for later calculations
            attempting_jump = player_inputs.rb || player_inputs.lt || player_inputs.a; //TODO determine best input
            firing_bullet = player_inputs.rt;

        }

        //Direction is based on way you're aiming if you're aiming
        if (fabs(player.gun_angle) < M_PI / 2) {
            player.facing_right = true;
        } else {
            player.facing_right = false;
        }

        //X-Speed considerations for stopping and turning around on the ground
        if (is_colliding_with_ground(player.x, player.y + 0.05, 0.5 - 0.1, 1 - 0.1)) {
            if (accel == 0) {
                player.dx = player.dx / 4;
                if (fabs(player.dx) < 0.2 * X_ACCEL) {
                    player.dx = 0;
                }
            } else if (fabs(player.dx + accel) < fabs(player.dx)) {
                player.dx = player.dx = (player.dx / 4) + accel;
            } else {
                player.dx += accel;
            }
        } else {
            //X-Speed considerations for aerial motion
            player.dx = player.dx + AERIAL_ACCEL_COEF * accel;
        }

        //Speed cap for horizontal motion
        //TODO figure out if we want to alter or remove the cap when not grounded
        if (fabs(player.dx) > MAX_X_SPEED){
            player.dx = (player.dx > 0) ? MAX_X_SPEED : -MAX_X_SPEED;
        }

        //Bullet logic
		if (player.ticks_till_next_bullet > 0) {
			player.ticks_till_next_bullet --;
		} else if (firing_bullet) {
			player.ticks_till_next_bullet = 15;

			Bullet next_bullet;
			next_bullet.player_owner = i;

			double scale_factor = player.facing_right ? 1 : -1;
			next_bullet.x = player.x + scale_factor * ((-15 + 26)/60.0) + 15.0/60 * cos(-17.0 * M_PI / 180.0 + player.gun_angle);
			next_bullet.y = player.y + (-30 + 35)/60.0 + 15.0/60 * sin(-17.0 * M_PI / 180.0 + player.gun_angle);

			next_bullet.x_vel = BULLET_VEL * cos(player.gun_angle);
			next_bullet.y_vel = BULLET_VEL * sin(player.gun_angle);

			bullets.push_back(next_bullet);
		}

        //Jumping/Gravity logic
        //Grounded
        if(is_colliding_with_ground(player.x, player.y + 0.05, 0.5 - 0.1, 1 - 0.1)){

            //Reset jumps
            player.double_jump = false;
            player.is_jumping = false;

            //Arrest fall
            player.dy = 0;

            //Standard-Jump
            if(attempting_jump){
                player.is_jumping = true;
                player.dy -= 1 * JUMP_COEF;
                player.ticks_left_jumping = JUMP_DIR;
            }

        //Wall-Cling
        } else if (is_colliding_with_ground(player.x + (accel > 0 ? 1 : -1) * 0.5, player.y, 0.5 - 0.1, 1 - 0.1)){

            //Reset jumps
            player.double_jump = false;
            player.is_jumping = false;

            //Arrest motion
            player.dx = 0;

            //Half speed fall, plus added friction based on the speed
            //TODO revisit formula
            player.dy = (player.dy + GRAVITY) / 2;

            //Wall-Jump
            if(attempting_jump){
                int dir = accel > 0 ? 1 : -1;
                player.is_jumping = true;
                player.dy -= 1 * JUMP_COEF;
                player.dx = dir * JUMP_COEF;
                player.ticks_left_jumping = JUMP_DIR;
            }

        //Aerial Logic
        } else {
            //Continued-Jump
            if(player.is_jumping){
                if(attempting_jump){
                    if (player.ticks_left_jumping >= 0) {
                        player.ticks_left_jumping--;
                        //To smoothly counteract the falling motion
                        player.dy -= GRAVITY; // * (player.ticks_left_jumping/(double)JUMP_DIR);
                    }
                } else {
                    player.is_jumping = false;
                    player.ticks_left_jumping = 0;
                }
            } else {
                //Double-Jump
                if(attempting_jump && !player.double_jump){
                    //Consume your double jump
                    player.double_jump = true;
                    player.is_jumping = true;
                    player.dy += 1 * JUMP_COEF;
                    player.ticks_left_jumping = JUMP_DIR; //TODO any tweaks necessary?

                }
            }
            //Gravity always has an effect in the air.
            player.dy += GRAVITY;
            if (fabs(player.dy) > MAX_Y_SPEED){
                player.dy = (player.dy > 0) ? MAX_Y_SPEED : -MAX_Y_SPEED;
            }
        }

        //Collision tracking super basic
        //TODO more nuanced approach
        bool loop = true;
        while(is_colliding_with_ground(player.x + player.dx, player.y + player.dy, 0.5 - 0.1, 1 - 0.1) && loop){
            loop = false;
            if(is_colliding_with_ground(player.x + player.dx, player.y, 0.5 - 0.1, 1 - 0.1)&& player.dx != 0){
                player.dx =  fabs(player.dx) < X_ACCEL ? 0 : player.dx - (player.dx > 0 ? X_ACCEL : -X_ACCEL);
                loop = true;
            }
            if(is_colliding_with_ground(player.x, player.y + player.dy, 0.5 - 0.1, 1 - 0.1) && player.dy != 0){
                player.dy = fabs(player.dy) < X_ACCEL ? 0 : player.dy - (player.dy > 0 ? X_ACCEL : -X_ACCEL);
                player.ticks_left_jumping = 0; //regardless of what's happening, this guy isn't going up anymore.
                loop = true;
            }
        }

        //Make motions as necessary
        player.x += player.dx;
        player.y += player.dy;

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

			if (rect_rect_colliding(bullet.x, bullet.y, 10/60.0, 10/60.0, player.x, player.y, 0.5, 1)) {
				hit_a_player = true;
				player.health -= 25;

				if (player.health == 0) {
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
	if (mouseX > (600 - 529)/2.0 && mouseX < (600 - 529)/2.0 + 529 && mouseY > (600 - 192)/2.0 && mouseY < (600 - 192)/2.0 + 192) {
		return nullptr;
	} else {
		return nullptr;
	}
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
		is_ground[(int)(x - width/2)][(int)(y - height/2)] || 
		is_ground[(int)(x + width/2)][(int)(y - height/2)] ||
		is_ground[(int)(x - width/2)][(int)(y + height/2)] ||
		is_ground[(int)(x + width/2)][(int)(y + height/2)]);
}