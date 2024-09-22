#include "../../engine.hpp"

#ifndef NURBS_HPP
#define NURBS_HPP

class NURBS : public Geometry
{
public:
    NURBS(std::vector<glm::vec4> points, int resolution);
    NURBS();
    ~NURBS();
};

#endif