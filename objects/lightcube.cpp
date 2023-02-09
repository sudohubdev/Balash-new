#include "lightcube.hpp"

LightCube::LightCube()
{
    this->geometry = new Geometry("assets/teapot.obj");
    this->texture = new Texture("assets/uvtest.png");
    this->position = glm::vec3(0, 0, 0);
    this->scale = glm::vec3(0.5, 0.5, 0.5);
    this->rotation = glm::vec3(0, 0, 0);
    this->genBuffers();
}

LightCube::~LightCube()
{
    //
}
