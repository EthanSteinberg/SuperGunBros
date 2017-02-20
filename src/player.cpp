
#include "player.h"

#include <cmath>

//TODO do i need the radius_2s? should they reflect the outline instead of the second radius?

const double arm_y_offset = -55 * ASSET_SCALE;

const double upper_arm_length = 32*ASSET_SCALE;
const double upper_arm_radius = 8*ASSET_SCALE;

const double lower_arm_length = 32*ASSET_SCALE;
const double lower_arm_radius = 5*ASSET_SCALE;

const double upper_leg_length = 40*ASSET_SCALE;
const double upper_leg_radius = 10*ASSET_SCALE;

const double lower_leg_length = 36*ASSET_SCALE;
const double lower_leg_radius = 8*ASSET_SCALE;

const double head_x_offset = 7 * ASSET_SCALE;
const double head_y_offset = -97 * ASSET_SCALE;

const double shoulder_x_offset = -2 * ASSET_SCALE;
const double shoulder_y_offset = -55 * ASSET_SCALE;
const double shoulder_angle_offset = M_PI / 2 + - 0.2;
const double shoulder_angle_coef = 1.0;

const double torso_y_offset = -40 * ASSET_SCALE;

const double crotch_x_offset = -18 * ASSET_SCALE;
const double crotch_y_offset = 0 * ASSET_SCALE;

inline double dist_sq(double x1, double y1, double x2, double y2) {
    return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
}

//calculates the angle opposite c in a triangle with sides a, b, and c
inline double calculate_angle_SSS(double a, double b, double c){
    //Use Law of cosines to get cos(C) and acos to get C
    if (c > a + b){
        printf("%f greater than %f+%f\n", c, a, b);
    }
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

std::vector<std::unique_ptr<Bullet>> Player::spawn_bullets() const {
    double scaled_gun_angle = is_facing_right() ? state.gun_angle : (M_PI - state.gun_angle);

    auto bullets = state.gun->spawn_bullets(scaled_gun_angle);

    for (auto& bullet : bullets) {
        if (!is_facing_right()) {
            bullet->pos.x *= -1;
        }

        bullet->pos.x += state.pos.x;
        bullet->pos.y += state.pos.y;
        bullet->angle += state.gun_angle;
    }

    return bullets;
}

void Player::render(RenderList& list) const {

    if (state.is_dead && state.ticks_until_spawn < DEATH_INVISIBLE_TIME) {
        return;
    }

    list.push();

    AnimationState interpolated = get_interpolated_frame();
    ArmState arms;

    double scaled_gun_angle = is_facing_right() ? state.gun_angle : (M_PI - state.gun_angle);

    double posX = state.pos.x;
    double posY = state.pos.y;

    double grip1_x = state.gun->grip1_x(scaled_gun_angle);
    double grip1_y = state.gun->grip1_y(scaled_gun_angle);

    double dist_to_grip1 = sqrt(dist_sq(grip1_x, grip1_y, 0, arm_y_offset));

    //Law of cosines
    arms.extra_angle[0] = calculate_angle_SSS(upper_arm_length, lower_arm_length, dist_to_grip1);
    arms.needed_angle[0] = atan2(grip1_y - arm_y_offset, grip1_x);

    double grip2_x = state.gun->grip2_x(scaled_gun_angle);
    double grip2_y = state.gun->grip2_y(scaled_gun_angle);

    double dist_to_grip2 = sqrt(dist_sq(grip2_x, grip2_y, 0, arm_y_offset));

    arms.extra_angle[1] = calculate_angle_SSS(upper_arm_length, lower_arm_length, dist_to_grip2);
    arms.needed_angle[1] = atan2(grip2_y - arm_y_offset, grip2_x) - M_PI/2;

    printf("DEBUG: grip1(x,y): (%f, %f), scaled_gun_angle: %f\n", grip1_x, grip1_y, scaled_gun_angle);

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

    list.add_image("black", -20, -62, 40, 8);
    list.add_image("black", -18, -60, 36, 4);

    double health = std::max(0.0, state.health);

    list.add_image(get_color_name(info.color), -18, -60, 36 * health / MAX_HEALTH, 4);

    for (int i = 0; i < state.kills; i++) {
        Rectangle life_box(-15 + i * 10, -67, 8, 8);
        list.add_rect("life-" + get_color_name(info.color), life_box);
    }

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
    draw_leg(0, list, interpolated);

    //BACK ARM
    //draw_arm(1, list, arms);

    //TODO revisit the crotch, it's not visible RN and not worth tweaking
    //CROTCH
    //list.add_scaled_image("torso-crotch", crotch_x_offset, crotch_y_offset, ASSET_SCALE);

    //FRONT LEG
    draw_leg(1, list, interpolated);

    //TORSO
    list.add_scaled_image("torso-main", 0, torso_y_offset, ASSET_SCALE, true);

    //FACE
    list.add_scaled_image(head, head_x_offset, head_y_offset, ASSET_SCALE, true);

    //JETPACK
    list.add_image("black", -18, -20, 8, 24);
    list.add_image(get_color_name(info.color), -16, -2 + -16 * state.fuel_left, 4, 20 * state.fuel_left);
    if (state.boosting) {
       list.add_image("fire", -17.5, 4);
    }

    //GUN
    if (state.gun->in_front()) {
        state.gun->render(list, scaled_gun_angle);
    }

    //FRONT ARM
    draw_arm(0, list, arms);

    //SHOULDER
    list.translate(shoulder_x_offset, shoulder_y_offset);
    list.rotate(shoulder_angle_coef * arms.needed_angle[1] + shoulder_angle_offset);
    list.add_scaled_image(shoulder, 0, 0, ASSET_SCALE, true);

    //FIX THAT STUFF
    list.pop();
}

void Player::draw_leg(int leg, RenderList &list, AnimationState interpolated) const {
    list.push();

    //OUTLINE
    list.rotate(interpolated.hip_angle[leg] * M_PI/180);

    list.translate(0, upper_leg_length);

    list.rotate(interpolated.knee_angle[leg] * M_PI/180);

    list.add_scaled_image("boot", 2.5, lower_leg_length + 5, ASSET_SCALE, true);
    list.add_scaled_image("leg-lower-outline", 0, (lower_leg_length - lower_leg_radius)/2, ASSET_SCALE,  true);


    list.rotate(-interpolated.knee_angle[leg] * M_PI/180);
    list.translate(0, -upper_leg_length);


    list.add_scaled_image("leg-upper-outline", 0, (upper_leg_length - upper_leg_radius)/2, ASSET_SCALE,  true);

    //FILL

    list.translate(0, upper_leg_length);

    list.rotate(interpolated.knee_angle[leg] * M_PI/180);

    list.add_scaled_image("leg-lower-fill", 0, (lower_leg_length - lower_leg_radius)/2, ASSET_SCALE,  true);


    list.rotate(-interpolated.knee_angle[leg] * M_PI/180);
    list.translate(0, -upper_leg_length);

    list.add_scaled_image("leg-upper-fill", 0, (upper_leg_length - upper_leg_radius)/2, ASSET_SCALE,  true);

    list.pop();
}

void Player::draw_arm(int arm, RenderList& list, ArmState arms) const{
    double shoulder_angle = arms.needed_angle[arm] - (M_PI - arms.extra_angle[arm])/2;
    {
        list.push();

        list.translate(0, arm_y_offset);
        list.rotate(shoulder_angle);

        list.add_scaled_image("arm-upper-outline", 0, (upper_arm_length - upper_arm_radius)/2, ASSET_SCALE,  true);

        list.translate(0, upper_arm_length);
        list.rotate(M_PI + arms.extra_angle[arm]);

        list.add_scaled_image("hand", 0, lower_arm_length + 2 * lower_arm_radius, ASSET_SCALE,  true);
        list.add_scaled_image("arm-lower-outline", 0, (lower_arm_length + lower_arm_radius)/2, ASSET_SCALE,  true);

        list.pop();
    }
    {
        list.push();

        list.translate(0, arm_y_offset);
        list.rotate(shoulder_angle);

        list.add_scaled_image("arm-upper-fill", 0, (upper_arm_length - upper_arm_radius)/2, ASSET_SCALE,  true);

        list.translate(0, upper_arm_length);
        list.rotate(M_PI + arms.extra_angle[arm]);

        list.add_scaled_image("arm-lower-fill", 0, (lower_arm_length + lower_arm_radius)/2, ASSET_SCALE,  true);

        list.pop();
    }

}

void Player::set_gun(std::unique_ptr<Gun> gun) {
    state.gun = std::move(gun);
    state.ammo_left = state.gun->initial_ammo();
    state.ticks_till_next_bullet = state.gun->ticks_between_shots();
}
