#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include "renderlist.h"

class Camera {
public:
    Camera(double level_width, double level_height);

    void transform(RenderList& list) const;
    void update(const std::vector<Point>& player_positions);

private:
    double x, y;
    double scale;

    double level_width, level_height;
};

#endif