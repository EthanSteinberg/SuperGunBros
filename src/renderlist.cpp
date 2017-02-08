#include "renderlist.h"

#include <glad/glad.h>

#include <fstream>

RenderList::RenderList(const char* filename) {
	std::ifstream file(filename);

	file >> metadata;

	std::array<std::array<float, 3>, 3> mat = {{
		{1, 0, 0},
		{0, 1, 0},
		{0, 0, 1}
	}};

	transform = mat;
}

Rectangle RenderList::get_image_dimensions(const std::string &name) const {
    auto info_iter = metadata.find(name);

    if (info_iter == metadata.end()) {
        std::cerr<<"Could not find image \"" << name << "\"" << std::endl;
        exit(-1);
    }

    auto& info = *info_iter;

    return {0, 0, info["sizex"], info["sizey"]};
}

void RenderList::add_outline(const std::string &name, const Rectangle& rect, double line_width) {
    add_line(name, rect.x - rect.width/2, rect.y - rect.height/2 + line_width / 2, rect.x + rect.width/2, rect.y - rect.height/2 + line_width/2);
    add_line(name, rect.x - rect.width/2, rect.y + rect.height/2 - line_width / 2, rect.x + rect.width/2, rect.y + rect.height/2 - line_width/2);
    add_line(name, rect.x - rect.width/2 + line_width / 2, rect.y - rect.height/2, rect.x - rect.width/2 + line_width / 2, rect.y + rect.height/2);
    add_line(name, rect.x + rect.width/2 - line_width / 2, rect.y - rect.height/2, rect.x + rect.width/2 - line_width / 2, rect.y + rect.height/2);
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

        auto& info = metadata[blah];

        add_image(blah, x, y - (double) info["sizey"]);
        x += (double) info["sizex"] + 4;
    }
}

void RenderList::add_image(const std::string &name, float x, float y, float width, float height) {
    auto info_iter = metadata.find(name);

    if (info_iter == metadata.end()) {
        std::cerr<<"Could not find image \"" << name << "\"" << std::endl;
        exit(-1);
    }

    auto& info = *info_iter;

    if (width == -1) {
        width = info["sizex"];
    }

    if (height == -1) {
        height = info["sizey"];
    }

    add_image_core(name, x, y, width, height);
}

void RenderList::add_scaled_image(const std::string &name, float x, float y, float scale, bool centered) {
    auto info_iter = metadata.find(name);

    if (info_iter == metadata.end()) {
        std::cerr<<"Could not find image \"" << name << "\"" << std::endl;
        exit(-1);
    }

    auto& info = *info_iter;

    float width = info["sizex"];
    width = width * scale;
    float height = info["sizey"];
    height = height * scale;

    if(centered){
        x = x - width/2;
        y = y - height/2;
    }

    add_image_core(name, x, y, width, height);
}

void RenderList::add_image_core(const std::string &name, float x, float y, float width, float height) {
    auto& info = metadata[name];

    if (info.is_null()) {
        std::cerr<<"Could not find image \"" << name << "\"" << std::endl;
        exit(-1);
    }

    int px = info["x"];
    int psizex = info["sizex"];

    int py = info["y"];
    int psizey = info["sizey"];

    add_transfored_point(x, y + height);
    data.push_back(0);

    data.push_back(px);
    data.push_back(py + psizey);

    data.push_back(px);
    data.push_back(py);
    data.push_back(psizex);
    data.push_back(psizey);


    add_transfored_point(x + width, y);
    data.push_back(0);

    data.push_back(px + psizex);
    data.push_back(py);

    data.push_back(px);
    data.push_back(py);
    data.push_back(psizex);
    data.push_back(psizey);


    add_transfored_point(x + width, y + height);
    data.push_back(0);

    data.push_back(px + psizex);
    data.push_back(py + psizey);

    data.push_back(px);
    data.push_back(py);
    data.push_back(psizex);
    data.push_back(psizey);


    add_transfored_point(x, y + height);
    data.push_back(0);

    data.push_back(px);
    data.push_back(py + psizey);

    data.push_back(px);
    data.push_back(py);
    data.push_back(psizex);
    data.push_back(psizey);


    add_transfored_point(x + width, y);
    data.push_back(0);

    data.push_back(px + psizex);
    data.push_back(py);

    data.push_back(px);
    data.push_back(py);
    data.push_back(psizex);
    data.push_back(psizey);


    add_transfored_point(x, y);
    data.push_back(0);

    data.push_back(px);
    data.push_back(py);

    data.push_back(px);
    data.push_back(py);
    data.push_back(psizex);
    data.push_back(psizey);
}

void RenderList::draw() {
	glBufferData(GL_ARRAY_BUFFER, 4 * data.size(), data.data(), GL_STREAM_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, data.size() / 9);
}

void RenderList::add_transfored_point(float x, float y) {
	float finalX = transform[0][0] * x + transform[0][1] * y + transform[0][2];
	float finalY = transform[1][0] * x + transform[1][1] * y + transform[1][2];

	data.push_back(finalX);
	data.push_back(finalY);
}

void RenderList::mmultiply(float other[][3]) {
    std::array<std::array<float, 3>, 3> result = {};

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			for (int k = 0; k < 3; k++) {
				result[i][j] += transform[i][k] * other[k][j];
			}
		}
	}

	transform = result;
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
    prev_states.push(transform);
}

void RenderList::pop() {
    transform = prev_states.top();
    prev_states.pop();
}