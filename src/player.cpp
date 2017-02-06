
#include "player.h"
#include "pistol.h"

const double arm_length = 9;
const double arm_radius = 2.5;

const double leg_length = 13;
const double leg_radius = 3;

const double arm_y_offset = -18;

inline double dist_sq(double x1, double y1, double x2, double y2) {
    return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
}

Player::Player(double start_x, double start_y, PlayerInfo a_info) : info(a_info), state(start_x, start_y) {
    current_time = 0;
    AnimationState frames_init[] = {
            {
                    {30, -30},
                    {20, 0},
            },
            {
                    {20, -20},
                    {30, 30},
            },
            {
                    {0, 0},
                    {90, 0},
            },
            {
                    {-20, 20},
                    {60, 0},
            },


            {
                    {-30, 30},
                    {0, 20},
            },
            {
                    {-20, 20},
                    {30, 30},
            },
            {
                    {0, 0},
                    {0, 90},
            },
            {
                    {20, -20},
                    {0, 60},
            },
    };

    frames = std::vector<AnimationState>(std::begin(frames_init), std::end(frames_init));

}

AnimationState Player::get_interpolated_frame() const {
    AnimationState current_frame = frames[(int) (current_time) % frames.size()];
    AnimationState next_frame = frames[(int) (current_time + 1) % frames.size()];

    double ratio = current_time - floor(current_time);

    AnimationState interpolated = {
            {current_frame.hip_angle[0] * (1 - ratio) + next_frame.hip_angle[0] * ratio, current_frame.hip_angle[1] * (1 - ratio) + next_frame.hip_angle[1] * ratio},
            {current_frame.knee_angle[0] * (1 - ratio) + next_frame.knee_angle[0] * ratio, current_frame.knee_angle[1] * (1 - ratio) + next_frame.knee_angle[1] * ratio},
    };

    return interpolated;
}

void Player::update() {
    if (state.dx == 0 || !state.grounded) {
        double dist_to_forward_back = fmod(current_time - 0.86 + 8, 8);
        double dist_to_backward_back = fmod(current_time - 3.5 + 8, 8);

        double dist_to_forward_front = fmod(-current_time + 0.86 + 8, 8);
        double dist_to_backward_front = fmod(-current_time + 3.5 + 8, 8);

        double min_distance = std::min(dist_to_forward_back, std::min(dist_to_forward_front, std::min(dist_to_backward_front, dist_to_backward_back)));

        if (dist_to_forward_front == min_distance) {
            current_time = std::min(fmod(current_time + 0.5 + 8, 8), 0.86);
        } else if (dist_to_forward_back == min_distance) {
            current_time = std::max(fmod(current_time - 0.5 + 8, 8), 0.86);
        } else if (dist_to_backward_front == min_distance) {
            current_time = std::min(fmod(current_time + 0.5 + 8, 8), 3.5);
        } else if (dist_to_backward_back == min_distance) {
            current_time = std::max(fmod(current_time - 0.5 + 8, 8), 3.5);
        }
    } else {
        current_time = fmod((current_time + state.dx * (is_facing_right() ? 1 : -1) * 0.10) + 8, 8);
        last_time_diff = state.dx * (is_facing_right() ? 1 : -1) * 0.10;
    }

    if (state.invincibility_ticks_left > 0) {
        state.invincibility_ticks_left--;
    }
}

bool Player::is_facing_right() const {
    return fabs(state.gun_angle) < M_PI / 2;
}

Bullet Player::spawn_bullet() const {
    double scaled_gun_angle = is_facing_right() ? state.gun_angle : (M_PI - state.gun_angle);

    Bullet b = state.gun->spawn_bullet(scaled_gun_angle);

    if (!is_facing_right()) {
        b.pos.x *= -1;
        b.x_vel *= -1;
    }

    b.pos.x += state.pos.x;
    b.pos.y += state.pos.y;

    return b;
}

void Player::render(RenderList& list) const {

    if (state.is_dead && state.ticks_until_spawn < DEATH_INVISIBLE_TIME) {
        return;
    }

    list.push();

    AnimationState interpolated = get_interpolated_frame();

    double scaled_gun_angle = is_facing_right() ? state.gun_angle : (M_PI - state.gun_angle);

    double posX = state.pos.x;
    double posY = state.pos.y;

    double grip1_x = state.gun->grip1_x(scaled_gun_angle);
    double grip1_y = state.gun->grip1_y(scaled_gun_angle);

    double dist_to_grip1 = sqrt(dist_sq(grip1_x, grip1_y, 0, arm_y_offset));

    double extra_angle1 = acos(dist_to_grip1 / (arm_length * 2));

    double needed_angle1 = atan2(grip1_y - arm_y_offset, grip1_x) - M_PI/2;

    double grip2_x = state.gun->grip2_x(scaled_gun_angle);
    double grip2_y = state.gun->grip2_y(scaled_gun_angle);

    double dist_to_grip2 = sqrt(dist_sq(grip2_x, grip2_y, 0, arm_y_offset));

    double extra_angle2 = acos(dist_to_grip2 / (arm_length * 2));

    double needed_angle2 = atan2(grip2_y - arm_y_offset, grip2_x) - M_PI/2;

    const char* head = nullptr;
    const char* shoulder = nullptr;

    switch (info.color) {
        case PlayerColor::RED:
            head = "red-head";
            shoulder = "red-shoulder";
            break;

        case PlayerColor::YELLOW:
            head = "yellow-head";
            shoulder = "yellow-shoulder";
            break;

        case PlayerColor::BLUE:
            head = "blue-head";
            shoulder = "blue-shoulder";
            break;

        case PlayerColor::GREEN:
            head = "green-head";
            shoulder = "green-shoulder";
            break;
    }


    list.translate(posX, posY);

    //Make this stuff half size by default?
    //list.scale(0.5, 0.5);

    if (!is_facing_right()) {
        list.scale(-1, 1);
    }

    if (state.invincibility_ticks_left > 0) {
        Rectangle shield_rect = list.get_image_dimensions("shield");
        list.add_rect("shield", shield_rect.offset(0, -2));
    }

    if (!state.gun->in_front()) {
        state.gun->render(list, scaled_gun_angle);
    }

    //BACK LEG
    {
        list.rotate(interpolated.hip_angle[0] * M_PI/180);


        list.translate(0, leg_length);

        list.rotate(interpolated.knee_angle[0] * M_PI/180);

        list.add_image("leg-upper-outline", -leg_radius, -leg_radius);
        list.add_image("leg-upper-fill", -leg_radius, -leg_radius);
        list.add_image("boot", -leg_radius, leg_length);

        list.rotate(-interpolated.knee_angle[0] * M_PI/180);
        list.translate(0, -leg_length);

        list.add_image("leg-lower-outline", -leg_radius, -leg_radius);
        list.add_image("leg-lower-fill", -leg_radius, -leg_radius);

        list.rotate(-interpolated.hip_angle[0] * M_PI/180);
    }

    //BACK ARM
    {
        list.translate(0, arm_y_offset);

        list.rotate(needed_angle2 + extra_angle2);
        list.add_image("arm-upper-outline", -arm_radius, -arm_radius);
        list.add_image("arm-upper-fill", -arm_radius, -arm_radius);

        list.translate(0, arm_length);

        list.rotate(-2 * extra_angle2);

        list.add_image("hand", -arm_length, -arm_radius);
        list.add_image("arm-lower-outline", -arm_radius, -arm_radius);
        list.add_image("arm-lower-fill", -arm_radius, -arm_radius);
        list.rotate(2 * extra_angle2);

        list.translate(0, -arm_length);

        list.rotate(-(needed_angle2 + extra_angle2));

        list.translate(0, -arm_y_offset);
    }

    //CROTCH
    list.add_image("torso-crotch", -7, 30);

    //FRONT LEG
    {
        list.rotate(interpolated.hip_angle[1] * M_PI/180);


        list.translate(0, leg_length);

        list.rotate(interpolated.knee_angle[1] * M_PI/180);

        list.add_image("leg-upper-outline", -leg_radius, -leg_radius);
        list.add_image("leg-upper-fill", -leg_radius, -leg_radius);
        list.add_image("boot", -leg_radius, leg_length);

        list.rotate(-interpolated.knee_angle[1] * M_PI/180);
        list.translate(0, -leg_length);

        list.add_image("leg-lower-outline", -leg_radius, -leg_radius);
        list.add_image("leg-lower-fill", -leg_radius, -leg_radius);

        list.rotate(-interpolated.hip_angle[1] * M_PI/180);
    }

    //TORSO
    list.add_image("torso-main", -7, -26);

    //FACE
    list.add_image(head, -6, -40);

    //JETPACK
    list.add_image("black", -14, -20, 6, 20);
    list.add_image(get_color_name(info.color), -12, -2 + -16 * state.fuel_left, 2, 16 * state.fuel_left);
    if (state.boosting) {
       list.add_image("fire", -13.5, 0);
    }

    //GUN
    if (state.gun->in_front()) {
        state.gun->render(list, scaled_gun_angle);
    }

    //FRONT ARM
    {
        list.translate(0, arm_y_offset);

        list.rotate(needed_angle2 + extra_angle2);
        list.add_image("arm-upper-outline", -arm_radius, -arm_radius);
        list.add_image("arm-upper-fill", -arm_radius, -arm_radius);

        list.translate(0, arm_length);

        list.rotate(-2 * extra_angle2);

        list.add_image("hand", -arm_length, -arm_radius);
        list.add_image("arm-lower-outline", -arm_radius, -arm_radius);
        list.add_image("arm-lower-fill", -arm_radius, -arm_radius);
        list.rotate(2 * extra_angle2);

        list.translate(0, -arm_length);

        list.rotate(-(needed_angle2 + extra_angle2));

        list.translate(0, -arm_y_offset);
    }

    //REVERSE REVERSE
    if (!is_facing_right()) {
        list.scale(-1, 1);
    }

    //list.scale(2, 2);

    //FIX THAT STUFF
    list.translate(-posX, -posY);

    list.pop();
}

