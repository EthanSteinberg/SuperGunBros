#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include "renderlist.h"

class Camera {
public:
    Camera();

    void transform(RenderList& list) const;
    void update(const std::vector<Point>& player_positions);

private:
    double x, y;
    double scale;
};

#endif