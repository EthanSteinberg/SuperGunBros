
#include "player.h"

//const double arm_length = 9;
//const double arm_radius = 2.5;
//
//const double leg_length = 13;
//const double leg_radius = 3;
//
//const double arm_y_offset = -18;

//TODO do i need the radius_2s? should they reflect the outline instead of the second radius?

const double arm_y_offset = -50 * base_scale;

const double upper_arm_length = 32*base_scale;
const double upper_arm_radius_1 = 8*base_scale;
const double upper_arm_radius_2 = 5*base_scale;

const double lower_arm_length = 32*base_scale;
const double lower_arm_radius_1 = 5*base_scale;
const double lower_arm_radius_2 = 0; //TODO: 4 was the sidewise dimension so keep that in mind

const double upper_leg_length = 40*base_scale;
const double upper_leg_radius_1 = 10*base_scale;
const double upper_leg_radius_2 = 8*base_scale;

const double lower_leg_length = 36*base_scale;
const double lower_leg_radius_1 = 8*base_scale;
const double lower_leg_radius_2 = 5*base_scale;

const double head_x_offset = -15 * base_scale;
const double head_y_offset = -100 * base_scale;

const double shoulder_x_offset = 0 * base_scale;
const double shoulder_y_offset = 0 * base_scale;
const double shoulder_angle = 0;

const double torso_x_offset = -16 * base_scale;
const double torso_y_offset = -75 * base_scale;
const double crotch_x_offset = -18 * base_scale;
const double crotch_y_offset = 0 * base_scale;

inline double dist_sq(double x1, double y1, double x2, double y2) {
    return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
}

//calculates the angle opposite c in a triangle with sides a, b, and c
inline double calculate_angle_SSS(double a, double b, double c){
    //Use Law of cosines to get cos(C) and acos to get C
    return acos((a*a + b*b - (c * c))/(2*a*b));
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

std::unique_ptr<Bullet> Player::spawn_bullet() const {
    double scaled_gun_angle = is_facing_right() ? state.gun_angle : (M_PI - state.gun_angle);

    std::unique_ptr<Bullet> b = state.gun->spawn_bullet(scaled_gun_angle);

    if (!is_facing_right()) {
        b->pos.x *= -1;
    }

    b->pos.x += state.pos.x;
    b->pos.y += state.pos.y;
    b->angle = state.gun_angle;

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

    //Law of cosines
    double extra_angle1 = calculate_angle_SSS(upper_arm_length, lower_arm_length, dist_to_grip1);

    double needed_angle1 = atan2(grip1_y - arm_y_offset, grip1_x) - M_PI/2;

    double grip2_x = state.gun->grip2_x(scaled_gun_angle);
    double grip2_y = state.gun->grip2_y(scaled_gun_angle);

    double dist_to_grip2 = sqrt(dist_sq(grip2_x, grip2_y, 0, arm_y_offset));

    double extra_angle2 = calculate_angle_SSS(upper_arm_length, lower_arm_length, dist_to_grip2);

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

    list.add_image("black", -20, -52, 40, 8);
    list.add_image("black", -18, -50, 36, 4);

    double health = std::max(0, state.health);

    list.add_image(get_color_name(info.color), -18, -50, 36 * health / MAX_HEALTH, 4);

    if (state.is_dead) {
        double angle = M_PI / 2.0 * (1.0 - ((state.ticks_until_spawn - DEATH_INVISIBLE_TIME) / (float)DEATH_ANIMATION_TIME));

        list.translate(0, 30);
        list.rotate(angle);
        list.translate(0, -30);
    }

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


        list.translate(0, lower_leg_length);

        list.rotate(interpolated.knee_angle[0] * M_PI/180);

        list.add_scaled_image("boot", -lower_leg_radius_1, lower_leg_length, base_scale);
        list.add_scaled_image("leg-lower-outline", -lower_leg_radius_1, -lower_leg_radius_1, base_scale);


        list.rotate(-interpolated.knee_angle[0] * M_PI/180);
        list.translate(0, -lower_leg_length);


        list.add_scaled_image("leg-upper-outline", -upper_leg_radius_1, -upper_leg_radius_1, base_scale);

        list.rotate(-interpolated.hip_angle[0] * M_PI/180);
    }
    {
        list.rotate(interpolated.hip_angle[0] * M_PI/180);


        list.translate(0, lower_leg_length);

        list.rotate(interpolated.knee_angle[0] * M_PI/180);

        list.add_scaled_image("leg-lower-fill", -lower_leg_radius_1, -lower_leg_radius_1, base_scale);


        list.rotate(-interpolated.knee_angle[0] * M_PI/180);
        list.translate(0, -lower_leg_length);

        list.add_scaled_image("leg-upper-fill", -upper_leg_radius_1, -upper_leg_radius_1, base_scale);

        list.rotate(-interpolated.hip_angle[0] * M_PI/180);
    }

    //BACK ARM
    {
        list.translate(0, arm_y_offset);

        list.rotate(needed_angle2 + extra_angle2);
        list.add_scaled_image("arm-upper-outline",-upper_arm_radius_1,-upper_arm_radius_1, base_scale);

        list.translate(0, upper_arm_length);

        list.rotate(-2 * extra_angle2);

        list.add_scaled_image("hand", -lower_arm_radius_1,lower_arm_length, base_scale);
        list.add_scaled_image("arm-lower-outline",-lower_arm_radius_1,-lower_arm_radius_1, base_scale);
        list.rotate(2 * extra_angle2);

        list.translate(0, -lower_arm_length);

        list.rotate(-(needed_angle2 + extra_angle2));

        list.translate(0, -arm_y_offset);
    }
    {
        list.translate(0, arm_y_offset);

        list.rotate(needed_angle2 + extra_angle2);
        list.add_scaled_image("arm-upper-fill",-upper_arm_radius_1,-upper_arm_radius_1, base_scale);

        list.translate(0, upper_arm_length);

        list.rotate(-2 * extra_angle2);

        list.add_scaled_image("arm-lower-fill",-lower_arm_radius_1,-lower_arm_radius_1, base_scale);
        list.rotate(2 * extra_angle2);

        list.translate(0, -lower_arm_length);

        list.rotate(-(needed_angle2 + extra_angle2));

        list.translate(0, -arm_y_offset);
    }

    //TODO revisit the crotch, it's not visible RN and not worth tweaking
    //CROTCH
    //list.add_scaled_image("torso-crotch", crotch_x_offset, crotch_y_offset, base_scale);

    //FRONT LEG
    {
        list.rotate(interpolated.hip_angle[1] * M_PI/180);


        list.translate(0, lower_leg_length);

        list.rotate(interpolated.knee_angle[1] * M_PI/180);

        list.add_scaled_image("boot", -lower_leg_radius_1, lower_leg_length, base_scale);
        list.add_scaled_image("leg-lower-outline", -lower_leg_radius_1, -lower_leg_radius_1, base_scale);

        list.rotate(-interpolated.knee_angle[1] * M_PI/180);
        list.translate(0, -lower_leg_length);


        list.add_scaled_image("leg-upper-outline", -upper_leg_radius_1, -upper_leg_radius_1, base_scale);

        list.rotate(-interpolated.hip_angle[1] * M_PI/180);
    }
    {
        list.rotate(interpolated.hip_angle[1] * M_PI/180);


        list.translate(0, lower_leg_length);

        list.rotate(interpolated.knee_angle[1] * M_PI/180);

        list.add_scaled_image("leg-lower-fill", -lower_leg_radius_1, -lower_leg_radius_1, base_scale);

        list.rotate(-interpolated.knee_angle[1] * M_PI/180);
        list.translate(0, -lower_leg_length);

        list.add_scaled_image("leg-upper-fill", -upper_leg_radius_1, -upper_leg_radius_1, base_scale);

        list.rotate(-interpolated.hip_angle[1] * M_PI/180);
    }

    //TORSO
    list.add_scaled_image("torso-main", torso_x_offset, torso_y_offset, base_scale);

    //FACE
    list.add_scaled_image(head, head_x_offset, head_y_offset, base_scale);

    //JETPACK
    list.add_image("black", -16, -20, 8, 24);
    list.add_image("fuel-color", -14, -2 + -16 * state.fuel_left, 4, 20 * state.fuel_left);
    if (state.boosting) {
       list.add_image("fire", -15.5, 4);
    }

    //GUN
    if (state.gun->in_front()) {
        state.gun->render(list, scaled_gun_angle);
    }

    //FRONT ARM
    {
        list.translate(0, arm_y_offset);

        list.rotate(needed_angle1 + extra_angle1);
        list.add_scaled_image("arm-upper-outline",-upper_arm_radius_1,-upper_arm_radius_1, base_scale);

        list.translate(0, upper_arm_length);

        list.rotate(-2 * extra_angle1);

        list.add_scaled_image("hand", -lower_arm_radius_1,lower_arm_length, base_scale);
        list.add_scaled_image("arm-lower-outline",-lower_arm_radius_1,-lower_arm_radius_1, base_scale);
        list.rotate(2 * extra_angle1);

        list.translate(0, -lower_arm_length);

        list.rotate(-(needed_angle1 + extra_angle1));

        list.translate(0, -arm_y_offset);
    }
    {
        list.translate(0, arm_y_offset);

        list.rotate(needed_angle1 + extra_angle1);
        list.add_scaled_image("arm-upper-fill",-upper_arm_radius_1,-upper_arm_radius_1, base_scale);

        list.translate(0, upper_arm_length);

        list.rotate(-2 * extra_angle1);

        list.add_scaled_image("arm-lower-fill",-lower_arm_radius_1,-lower_arm_radius_1, base_scale);
        list.rotate(2 * extra_angle1);

        list.translate(0, -lower_arm_length);

        list.rotate(-(needed_angle1 + extra_angle1));

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

