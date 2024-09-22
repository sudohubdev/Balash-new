#include "tess.hpp"

using namespace std;



Bezier::Bezier(vector<glm::vec2> points)
{
    this->points = points;
    this->shader = LoadShaders("shaders/simple.vert", "shaders/simple.frag", "shaders/bezier.geom", "shaders/bezier.tcs.glsl", "shaders/bezier.tes.glsl");
    this->genBuffers();
}

float vertices[] = {
    -0.5f, -0.2f, 0.2f, //P0
    -0.2f,  0.44f, 0.8f, //P1
     0.3f, -0.55f, -0.25f, //P2
     0.1f,  0.7f, 0.3f //P3

};
void Bezier::genBuffers()
{
    // Bezier VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    // Bezier VBO
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

void Bezier::attachShader(GLuint shader)
{
    this->shader = shader;
}
GLuint Bezier::getShader()
{
    return this->shader;
}
void Bezier::bindBuffers()
{
    glUseProgram(this->getShader());
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    // disable mask
    glDepthMask(GL_FALSE);
}

void Bezier::setMVP(Camera *camera)
{
    glm::mat4 view = glm::mat4(glm::mat3(camera->getView()));
    glm::mat4 projection = camera->getProjection();
    glm::mat4 MVP = projection * view;
    //glUniformMatrix4fv(glGetUniformLocation(this->shader, "MVP"), 1, GL_FALSE, &MVP[0][0]);
}

GLsizei Bezier::getVertexCount()
{
    return 4;
}

void Bezier::draw(Camera *camera){
    bindBuffers();
    //setMVP(camera);
    // set uniforms
    //GLuint ID = glGetUniformLocation(getShader(), "time");
    //glUniform1f(ID, (float)glfwGetTime());
    //ID = glGetUniformLocation(getShader(), "camPos");
    //glUniform3f(ID, camera->position.x, camera->position.y, camera->position.z);
    glPatchParameteri(GL_PATCH_VERTICES, 4);
    //for(size_t i=0;i<points.size() /4; ++i){
    //    glDrawArrays(GL_PATCHES, i*4, 4);
    //}
    glDrawArrays(GL_PATCHES, 0, 4);
    unbindBuffers();
}

void Bezier::unbindBuffers()
{
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
    // unuse shader
    glUseProgram(0);
}

Bezier::~Bezier()
{
    glDeleteBuffers(1, &vao);
}