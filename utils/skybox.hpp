#include "../libs.hpp"
#include "../engine.hpp"
using namespace std;

#ifndef SKYBOX_HPP
#define SKYBOX_HPP

class Skybox : public Mesh
{
public:
    Skybox(vector<string> faces);
    void bindBuffers();
    void unbindBuffers();
    ~Skybox();
    void Render(Renderer *renderer, Camera *camera);
    void genBuffers();
    void attachShader(GLuint shader);
    GLuint getShader();
    void setMVP(Camera *camera);
    GLsizei getVertexCount();
    void draw();

protected:
    vector<string> faces;
    GLuint cubemapTexture;
    GLuint vao;
};

#endif