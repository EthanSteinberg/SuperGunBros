#include "renderlist.h"

#include <glad/glad.h>
#include <cmath>

#include <iostream>

RenderList::RenderList(const char* filename): file(filename), wrapper(file) {


    metadata.ParseStream(wrapper);

	std::array<std::array<float, 3>, 3> mat = {{
		{1, 0, 0},
		{0, 1, 0},
		{0, 0, 1}
	}};

    state.transform = mat;
    state.z_offset = 0;
}

Rectangle RenderList::get_image_dimensions(const std::string &name) const {
    if (!metadata.HasMember(name.c_str())) {
        std::cerr<<"Could not find image \"" << name << "\"" << std::endl;
        exit(-1);
    }

    auto& info = metadata[name.c_str()];

    return {0, 0, info["sizex"].GetDouble(), info["sizey"].GetDouble()};
}

void RenderList::add_outline(const std::string &name, const Rectangle& rect, double line_width) {
    add_line(name, rect.x - rect.width/2, rect.y - rect.height/2 + line_width / 2, rect.x + rect.width/2, rect.y - rect.height/2 + line_width/2, line_width);
    add_line(name, rect.x - rect.width/2, rect.y + rect.height/2 - line_width / 2, rect.x + rect.width/2, rect.y + rect.height/2 - line_width/2, line_width);
    add_line(name, rect.x - rect.width/2 + line_width / 2, rect.y - rect.height/2, rect.x - rect.width/2 + line_width / 2, rect.y + rect.height/2, line_width);
    add_line(name, rect.x + rect.width/2 - line_width / 2, rect.y - rect.height/2, rect.x + rect.width/2 - line_width / 2, rect.y + rect.height/2, line_width);
}

void RenderList::add_rect(const std::string &name, const Rectangle& rect) {
    add_image(name, rect.x - rect.width/2, rect.y - rect.height/2, rect.width, rect.height);
}

void RenderList::add_line(const std::string &name, float x_1, float y_1, float x_2, float y_2, double line_width) {
    double dist = sqrt((x_2 - x_1) * (x_2 - x_1) + (y_2 - y_1) * (y_2 - y_1));
    double angle = atan2(y_2 - y_1, x_2 - x_1);

    translate(x_1, y_1);
    rotate(angle);
    add_image(name, 0, -line_width/2, dist, line_width);
    rotate(-angle);
    translate(-x_1, -y_1);
}

void RenderList::add_number(float x, float y, int num) {
    std::string num_text = std::to_string(num);

    for (const char c : num_text) {
        std::string blah = "0";

        blah[0] = c;

        auto& info = metadata[blah.c_str()];

        add_image(blah, x, y - info["sizey"].GetDouble());
        x += (double) info["sizex"].GetDouble() + 4;
    }
}

void RenderList::add_image(const std::string &name, float x, float y, float width, float height) {
    if (!metadata.HasMember(name.c_str())) {
        std::cerr<<"Could not find image \"" << name << "\"" << std::endl;
        exit(-1);
    }

    auto& info = metadata[name.c_str()];

    if (width == -1) {
        width = info["sizex"].GetDouble();
    }

    if (height == -1) {
        height = info["sizey"].GetDouble();
    }

    add_image_core(name, x, y, width, height);
}

void RenderList::add_scaled_image(const std::string &name, float x, float y, float scale, bool centered) {
    if (!metadata.HasMember(name.c_str())) {
        std::cerr<<"Could not find image \"" << name << "\"" << std::endl;
        exit(-1);
    }

    auto& info = metadata[name.c_str()];

    float width = info["sizex"].GetDouble();
    width = width * scale;
    float height = info["sizey"].GetDouble();
    height = height * scale;

    if(centered){
        x = x - width/2;
        y = y - height/2;
    }

    add_image_core(name, x, y, width, height);
}

void RenderList::add_text(const std::string& text, float x, float y) {

    std::string lowercase = "abcdefghijklmnopqrstuvwxyz";
    std::string uppercase = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    const int advance = 11;
    const int height = 20;

    for (unsigned int i = 0; i < text.size(); i++) {
        char current_char = text[i];

        if (current_char == ' ') {
            // Don't draw anything for a space
        } else if (lowercase.find(current_char) != std::string::npos) {
            int offset = lowercase.find(current_char);
            add_image_core("lowercase", x, y, advance, height, offset * advance, 0, advance, height);
        } else if (uppercase.find(current_char) != std::string::npos) {
            int offset = uppercase.find(current_char);
            add_image_core("uppercase", x, y, advance, height, offset * advance, 0, advance, height);
        } else {
            std::cout<<"Could not find image for character " << current_char << std::endl;
        }

        x += advance;
    }
}

void RenderList::add_red_numbers(const std::string& color, int num, float center_x, float center_y) {
    std::string num_text = std::to_string(num);

    std::string numbers = "0123456789";

    const int advance = 24;
    const int height = 33;

    const int visual_advance = 21;

    float y = center_y - height / 2.0;
    float x = center_x - visual_advance * num_text.size() / 2.0;

    for (unsigned int i = 0; i < num_text.size(); i++) {
        char current_char = num_text[i];
        int offset = numbers.find(current_char);

        // add_image("green", x, y, visual_advance, height);
        add_image_core(color + "-numbers", x - 3, y, advance, height, offset * advance, 0, advance, height);

        x += visual_advance;
    }
}

void RenderList::add_image_core(const std::string &name, float x, float y, float width, float height) {
    if (!metadata.HasMember(name.c_str())) {
        std::cerr<<"Could not find image \"" << name << "\"" << std::endl;
        exit(-1);
    }

    auto& info = metadata[name.c_str()];
    add_image_core(name, x, y, width, height, 0, 0,  info["sizex"].GetInt(),  info["sizey"].GetInt());
}


void RenderList::add_image_core(const std::string &name, float x, float y, float width, float height, float subx, float suby, float subwidth, float subheight) {
    if (!metadata.HasMember(name.c_str())) {
        std::cerr<<"Could not find image \"" << name << "\"" << std::endl;
        exit(-1);
    }

    std::vector<float>* vec = &data;

    if (state.z_offset == 100) {
        vec = &top_data;
    }

    auto& info = metadata[name.c_str()];

    int px = info["x"].GetInt() + subx;
    int psizex = subwidth;

    int py = info["y"].GetInt() + suby;
    int psizey = subheight;

    add_transformed_point(*vec, x, y + height);
    vec->push_back(state.z_offset);

    vec->push_back(px);
    vec->push_back(py + psizey);

    vec->push_back(px);
    vec->push_back(py);
    vec->push_back(psizex);
    vec->push_back(psizey);


    add_transformed_point(*vec, x + width, y);
    vec->push_back(state.z_offset);

    vec->push_back(px + psizex);
    vec->push_back(py);

    vec->push_back(px);
    vec->push_back(py);
    vec->push_back(psizex);
    vec->push_back(psizey);


    add_transformed_point(*vec, x + width, y + height);
    vec->push_back(state.z_offset);

    vec->push_back(px + psizex);
    vec->push_back(py + psizey);

    vec->push_back(px);
    vec->push_back(py);
    vec->push_back(psizex);
    vec->push_back(psizey);


    add_transformed_point(*vec, x, y + height);
    vec->push_back(state.z_offset);

    vec->push_back(px);
    vec->push_back(py + psizey);

    vec->push_back(px);
    vec->push_back(py);
    vec->push_back(psizex);
    vec->push_back(psizey);


    add_transformed_point(*vec, x + width, y);
    vec->push_back(state.z_offset);

    vec->push_back(px + psizex);
    vec->push_back(py);

    vec->push_back(px);
    vec->push_back(py);
    vec->push_back(psizex);
    vec->push_back(psizey);


    add_transformed_point(*vec, x, y);
    vec->push_back(state.z_offset);

    vec->push_back(px);
    vec->push_back(py);

    vec->push_back(px);
    vec->push_back(py);
    vec->push_back(psizex);
    vec->push_back(psizey);
}

void RenderList::add_flame(float center_x, float center_y, float r, float g, float b, float a) {

    double radius = 8;
    double height = 20;
    double width = 20;

    double x = center_x - width / 2;
    double y = center_y - height / 2;

    add_transformed_point(flame_data, x, y + height);
    flame_data.push_back(state.z_offset);
    flame_data.push_back(r);
    flame_data.push_back(g);
    flame_data.push_back(b);
    flame_data.push_back(a);
    add_transformed_point(flame_data, center_x, center_y);
    flame_data.push_back(radius * state.transform[0][0]);


    add_transformed_point(flame_data, x + width, y);
    flame_data.push_back(state.z_offset);
    flame_data.push_back(r);
    flame_data.push_back(g);
    flame_data.push_back(b);
    flame_data.push_back(a);
    add_transformed_point(flame_data, center_x, center_y);
    flame_data.push_back(radius * state.transform[0][0]);

    add_transformed_point(flame_data, x + width, y + height);
    flame_data.push_back(state.z_offset);
    flame_data.push_back(r);
    flame_data.push_back(g);
    flame_data.push_back(b);
    flame_data.push_back(a);
    add_transformed_point(flame_data, center_x, center_y);
    flame_data.push_back(radius * state.transform[0][0]);

    add_transformed_point(flame_data, x, y + height);
    flame_data.push_back(state.z_offset);
    flame_data.push_back(r);
    flame_data.push_back(g);
    flame_data.push_back(b);
    flame_data.push_back(a);
    add_transformed_point(flame_data, center_x, center_y);
    flame_data.push_back(radius * state.transform[0][0]);

    add_transformed_point(flame_data, x + width, y);
    flame_data.push_back(state.z_offset);
    flame_data.push_back(r);
    flame_data.push_back(g);
    flame_data.push_back(b);
    flame_data.push_back(a);
    add_transformed_point(flame_data, center_x, center_y);
    flame_data.push_back(radius * state.transform[0][0]);

    add_transformed_point(flame_data, x, y);
    flame_data.push_back(state.z_offset);
    flame_data.push_back(r);
    flame_data.push_back(g);
    flame_data.push_back(b);
    flame_data.push_back(a);
    add_transformed_point(flame_data, center_x, center_y);
    flame_data.push_back(radius * state.transform[0][0]);
}

void RenderList::draw() {
	glBufferData(GL_ARRAY_BUFFER, 4 * data.size(), data.data(), GL_STREAM_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, data.size() / 9);
}

void RenderList::draw_flame() {
    glBufferData(GL_ARRAY_BUFFER, 4 * flame_data.size(), flame_data.data(), GL_STREAM_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, flame_data.size() / 10);
}

void RenderList::draw_top() {
    glBufferData(GL_ARRAY_BUFFER, 4 * top_data.size(), top_data.data(), GL_STREAM_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, top_data.size() / 9);
}

void RenderList::add_transformed_point(std::vector<float>& vec, float x, float y) {
	float finalX = state.transform[0][0] * x + state.transform[0][1] * y + state.transform[0][2];
	float finalY = state.transform[1][0] * x + state.transform[1][1] * y + state.transform[1][2];

	vec.push_back(finalX);
	vec.push_back(finalY);
}

void RenderList::mmultiply(float other[][3]) {
    std::array<std::array<float, 3>, 3> result = {};

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			for (int k = 0; k < 3; k++) {
				result[i][j] += state.transform[i][k] * other[k][j];
			}
		}
	}

	state.transform = result;
}

void RenderList::translate(float x, float y) {

	float mat[3][3] = {
		{1, 0, x},
		{0, 1, y},
		{0, 0, 1}
	};

	mmultiply(mat);
}

void RenderList::rotate(double radians) {
	float mat[3][3] = {
		{(float) cos(radians), (float) -sin(radians), 0},
		{(float) sin(radians), (float) cos(radians), 0},
		{0, 0, 1}
	};

	mmultiply(mat);
}

void RenderList::scale(float scaleX, float scaleY) {
	float mat[3][3] = {
		{scaleX, 0, 0},
		{0, scaleY, 0},
		{0, 0, 1}
	};

	mmultiply(mat);
}

void RenderList::push() {
    prev_states.push(state);
}

void RenderList::pop() {
    state = prev_states.top();
    prev_states.pop();
}

void RenderList::set_z(float z) {
    state.z_offset = z;
}