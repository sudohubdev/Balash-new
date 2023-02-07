#include "../libs.hpp"
#include "../engine.hpp"
using namespace std;

#ifndef SKYBOX_HPP
#define SKYBOX_HPP

class Skybox : protected Mesh
{
public:
    Skybox(vector<string> faces);
    void bindBuffers();
    void unbindBuffers();
    ~Skybox();
    void Render(Renderer *renderer, Camera *camera);
    void genBuffers();

protected:
    vector<string> faces;
    GLuint cubemapTexture;
    GLuint vao;
};

#endif