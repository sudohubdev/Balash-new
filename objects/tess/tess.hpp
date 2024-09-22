#include "../../libs.hpp"
#include "../../engine.hpp"
using namespace std;

#ifndef tess_HPP
#define tess_HPP

class Bezier : public Mesh
{
public:
    Bezier(vector<glm::vec2> points);
    void bindBuffers();
    void unbindBuffers();
    ~Bezier();
    void Render(Renderer *renderer, Camera *camera);
    void genBuffers();
    void attachShader(GLuint shader);
    GLuint getShader();
    void setMVP(Camera *camera);
    GLsizei getVertexCount();
    void draw(Camera *camera);

protected:
    vector<glm::vec2> points;
};

#endif