#include "../../libs.hpp"
#include "../../engine.hpp"
using namespace std;

#ifndef particle_HPP
#define particle_HPP

struct ParticleVBO {
    glm::vec3 position;
    glm::vec3 velocity;
    float lifeTime;
    glm::vec4 color;
};    


class Particle : public Mesh
{
public:
    Particle();
    void bindBuffers();
    void unbindBuffers();
    ~Particle();
    void Render(Renderer *renderer, Camera *camera);
    void genBuffers();
    void attachShader(GLuint shader);
    GLuint getShader();
    void setMVP(GLuint shaderID, glm::mat4 MVP);
    void setMVP(Camera *camera);
    GLsizei getVertexCount();
    void draw(Camera *camera);
    void update(float deltaTime);
protected:
    std::vector<ParticleVBO> particles;
    int maxParticles = 1000;
    float emissionRate = 100.0f; // particles per second
    float currentEmission = 0.0f;
};


#endif