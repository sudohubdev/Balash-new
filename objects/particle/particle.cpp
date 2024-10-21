#include "particle.hpp"

using namespace std;



Particle::Particle()
{
    this->shader = LoadShaders("shaders/particle/particle.vert", "shaders/particle/particle.frag", "shaders/particle/particle.geom");//, "shaders/particle/particle.tcs.glsl", "shaders/particle/particle.tes.glsl");
    particles.resize(maxParticles);
    this->genBuffers();
}

void Particle::genBuffers()
{
    particles.resize(maxParticles);
    for (auto& particle : particles) {
        particle.position = glm::vec3(0.0f);
        particle.velocity = glm::vec3(0.0f);
        particle.lifeTime = 0.0f;
        particle.color = glm::vec4(1.0f);
    }

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(ParticleVBO), &particles[0], GL_DYNAMIC_DRAW);
}

void Particle::attachShader(GLuint shader)
{
    this->shader = shader;
}
GLuint Particle::getShader()
{
    return this->shader;
}
void Particle::bindBuffers()
{
    glUseProgram(this->getShader());
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size() * sizeof(ParticleVBO), &particles[0]);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleVBO), (void*)offsetof(ParticleVBO, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleVBO), (void*)offsetof(ParticleVBO, velocity));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleVBO), (void*)offsetof(ParticleVBO, lifeTime));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleVBO), (void*)offsetof(ParticleVBO, color));

    // disable mask
    //glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
}

// does almost everything needed with matrices
void Particle::setMVP(GLuint shaderID, glm::mat4 MVP)
{
    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    GLuint MatrixID = glGetUniformLocation(shaderID, "MVP");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
}
// does almost everything needed with matrices
void Particle::setMVP(Camera *camera)
{
    camera->updateProjection();
    camera->updateView();
    glm::mat4 MVP = camera->getProjection() * camera->getView() * this->getModelMatrix();
    this->setMVP(this->shader, MVP);
    GLuint MatrixID = glGetUniformLocation(this->getShader(), "model");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &this->getModelMatrix()[0][0]);
}

GLsizei Particle::getVertexCount()
{
    return particles.size();
}

void Particle::draw(Camera *camera){
    glUseProgram(this->shader);
    setMVP(camera);
    bindBuffers();
   

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glPointSize(5.0f);
    glDrawArrays(GL_POINTS, 0, particles.size());

    // don't forget to reset to default blending mode
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    unbindBuffers();
}

void Particle::update(float deltaTime) {
    currentEmission += emissionRate * deltaTime;

    for (int i = 0; i < currentEmission && i < maxParticles; ++i) {
        ParticleVBO& p = particles[i];

        // Update the particle's position based on its velocity
        p.position += p.velocity * deltaTime;

        // Decrease particle's life
        p.lifeTime -= deltaTime;

        // Update color and transparency based on lifetime
        p.color.a = glm::max(0.0f, p.lifeTime / 1.0f); // Fades out over time

        if (p.lifeTime <= 0.0f) {
            // Respawn the particle
            p.position = glm::vec3(0.0f); // Start from the origin (adjust as necessary)
            p.velocity = glm::vec3((rand() % 100 - 50) / 50.0f, (rand() % 100) / 50.0f, (rand() % 100 - 50) / 50.0f);
            p.lifeTime = 1.0f; // Reset lifetime to 1 second
            p.color = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f); // Fire color (orange)
        }
    }
}



void Particle::unbindBuffers()
{
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
    // unuse shader
    glUseProgram(0);
}

Particle::~Particle()
{
    glDeleteBuffers(1, &vao);
}