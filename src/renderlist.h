#ifndef RENDERLIST_H_INCLUDED
#define RENDERLIST_H_INCLUDED

#include "rapidjson/document.h"
#include <array>
#include <vector>
#include <string>
#include <stack>

#include "rectangle.h"

#include <rapidjson/istreamwrapper.h>

#include <fstream>

/**
 * This class allows you to render images to the screen via OpenGL.
 * The basic idea is that you apply stateful transformations to the render list which affect all
 * subsequent add_image draws.
 */

class RenderList {
public:
	// Create the RenderList. The jsonFileName should be the appropriate metatadata JSON for the image atlas.
	RenderList(const char* jsonFileName);

	// Render a single image with the given name.
	// x and y are the coordinates to render to.
	// Width and height are optional, and default to the width and height of the source image.
	void add_image(const std::string &name, float x, float y, float width = -1, float height = -1);

    //Render a single image with width and height derived from one scale factor
    void add_scaled_image(const std::string &name, float x, float y, float scale, bool centered = false);

	void add_line(const std::string &name, float x_1, float y_1, float x_2, float y_2, double line_width = 4);

	void add_outline(const std::string &name, const Rectangle& rect, double line_width = 4);
	void add_rect(const std::string &name, const Rectangle& rect);

	void add_number(float x, float y, int num);

	// Perform a stateful translation by x and y.
	void translate(float x, float y);

	// Perform a stateful rotation by the provided radians.
	void rotate(double radians);

	// Scale the image by the appropriate scaleX and scaleY.
	void scale(float scaleX, float scaleY);

	Rectangle get_image_dimensions(const std::string &name) const;

	// Actually draw everything currently in the render list.
	void draw();


	void push();
	void pop();
private:

    void add_image_core(const std::string &name, float x, float y, float width, float height);

	std::array<std::array<float, 3>, 3> transform;
	std::stack<std::array<std::array<float, 3>, 3>> prev_states;

	void add_transfored_point(float x, float y);

	void mmultiply(float other[][3]);

	std::vector<float> data;

	rapidjson::Document metadata;

	std::ifstream file;

    rapidjson::BasicIStreamWrapper<std::ifstream> wrapper;
};

#endif