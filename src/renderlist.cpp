#include "renderlist.h"

#include <glad/glad.h>

#include <fstream>

RenderList::RenderList(const char* filename) {
	std::ifstream file(filename);

	file >> metadata;

	float mat[3][3] = {
		{1, 0, 0},
		{0, 1, 0},
		{0, 0, 1}
	};

	memcpy(transform, mat, sizeof(mat));
}

void RenderList::add_line(const char* color, float x_1, float y_1, float x_2, float y_2) {
    double dist = sqrt((x_2 - x_1) * (x_2 - x_1) + (y_2 - y_1) * (y_2 - y_1));
    double angle = atan2(y_2 - y_1, x_2 - x_1);

    translate(x_1, y_1);
    rotate(angle);
    add_image("black", 0, -2, dist, 4);
    rotate(-angle);
    translate(-x_1, -y_1);
}

void RenderList::add_image(const char* name, float x, float y, float width, float height) {
	auto& info = metadata[name];

    if (info.is_null()) {
        std::cerr<<"Could not find image \"" << name << "\"" << std::endl;
        exit(-1);
    }

	if (width == 0) {
		width = info["sizex"];
	}

	if (height == 0) {
		height = info["sizey"];
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
	float result[3][3] = {};

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			for (int k = 0; k < 3; k++) {
				result[i][j] += transform[i][k] * other[k][j];
			}
		}
	}

	memcpy(transform, result, sizeof(result));
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