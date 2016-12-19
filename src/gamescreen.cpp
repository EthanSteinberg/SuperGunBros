#include "gamescreen.h"

#include "gameoverscreen.h"

#include <cmath>

GameScreen::GameScreen(const std::vector<PlayerInfo>& infos) {

	// Initialize the player state
	for (unsigned int i = 0; i < infos.size(); i++) {
		const auto& info = infos[i];
		PlayerState player = {
			0,8.5, false, info, 0, true, 0, 100
		};
		switch (i) {
			case 0:
				player.x = 1.25;
				break;

			case 1:
				player.x = 8.75;
				break;
		}
		players.push_back(player);
	}

	// Initialize the map
	for (int x = 0; x < 10; x++) {
		for (int y = 0; y < 10; y++) {

			if (x == 0 || x == 9 || y == 0 || y == 9) {
				// Add borders around the map
				is_ground[x][y] = true;
			} else if (x >=3 && x <= 5 && y == 7) {
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
	for (int x = 0; x < 10; x++) {
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

		list.translate(-15 + 26, -30 + 35);
		list.rotate(player.gun_angle);
		list.add_image("simpleGun", -2, -5);
		list.rotate(-player.gun_angle);
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
		auto& player = players[i];
		double dx = 0;
		double dy = 0;

		bool firing_bullet;

		// Deal with player input.
		if (player.info.type == PlayerType::KEYBOARD) {
			if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
				dx -= 0.05;
			}
			if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
				dx += 0.05;
			}

			if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && is_colliding_with_ground(player.x, player.y + 0.05, 0.5 - 0.1, 1 - 0.1) && player.ticks_left_jumping == 0) {
				player.ticks_left_jumping = 30;
			}

			if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
				player.gun_angle += 0.1;
			}

			if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
				player.gun_angle -= 0.1;
			}

			firing_bullet = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
		} else if (player.info.type == PlayerType::GAMEPAD) {
			int count;
			const float *axis = glfwGetJoystickAxes(player.info.joystick_index, &count);
			dx += axis[4] * 0.05;

			player.gun_angle += axis[3] * 0.1;

			const unsigned char *buttons = glfwGetJoystickButtons(player.info.joystick_index, &count);

			if (buttons[4] && is_colliding_with_ground(player.x, player.y + 0.05, 0.5 - 0.1, 1 - 0.1) && player.ticks_left_jumping == 0) {
				player.ticks_left_jumping = 30;
			}

			firing_bullet = buttons[5];
		}

		if (dx > 0) {
			player.facing_right = true;
		} else if (dx < 0) {
			player.facing_right = false;
		}

		player.gun_angle = std::max(std::min(player.gun_angle, M_PI / 3), -M_PI / 3);

		if (player.ticks_left_jumping > 0) {
			player.ticks_left_jumping --;
			dy = -0.07;
		} else {
			dy = 0.05;
		}

		if (player.ticks_till_next_bullet > 0) {
			player.ticks_till_next_bullet --;
		} else if (firing_bullet) {
			player.ticks_till_next_bullet = 10;

			Bullet next_bullet;
			next_bullet.player_owner = i;

			double scale_factor = player.facing_right ? 1 : -1;
			next_bullet.x = player.x + scale_factor * ((-15 + 26)/60.0 + 15.0/60 * cos(-17.0 * M_PI / 180.0 + player.gun_angle));
			next_bullet.y = player.y + (-30 + 35)/60.0 + 15.0/60 * sin(-17.0 * M_PI / 180.0 + player.gun_angle);

			next_bullet.x_vel = 0.05 * scale_factor * cos(player.gun_angle);
			next_bullet.y_vel = 0.05 * sin(player.gun_angle);

			bullets.push_back(next_bullet);
		}

		if (!is_colliding_with_ground(player.x + dx, player.y + dy, 0.5 - 0.1, 1 - 0.1)) {
			player.x += dx;
			player.y += dy;
		} else if (!is_colliding_with_ground(player.x, player.y + dy, 0.5 - 0.1, 1 - 0.1)) {
			player.y += dy;
		} else if (!is_colliding_with_ground(player.x + dx, player.y, 0.5 - 0.1, 1 - 0.1)) {
			player.x += dx;
			player.ticks_left_jumping = 0;
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

		if (hit_a_player) {
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