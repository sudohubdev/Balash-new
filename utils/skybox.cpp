#include "skybox.hpp"

using namespace std;

class SkyboxTexture : public Texture
{
public:
    SkyboxTexture(vector<string> faces) : Texture()
    {
        this->textureID = loadpngSkybox(faces);
    }
};

static const GLfloat g_vertex_buffer_data[] = {
    -1.0f, 1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,

    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,

    -1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, 1.0f};

Skybox::Skybox(vector<string> faces)
{
    this->faces = faces;
    this->drawtype = 0;
    this->texture = new SkyboxTexture(faces);
    this->genBuffers();
}

void Skybox::genBuffers()
{
    // Skybox VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    // Skybox VBO
    glGenBuffers(1, &vao);
    glBindBuffer(GL_ARRAY_BUFFER, vao);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    glBindVertexArray(0);
}

void Skybox::attachShader(GLuint shader)
{
    this->shader = shader;
}
GLuint Skybox::getShader()
{
    return this->shader;
}
void Skybox::bindBuffers()
{
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture->getTextureID());
    // use shader
    glUseProgram(this->shader);
    // disable mask
    glDepthMask(GL_FALSE);
}

void Skybox::setMVP(Camera *camera)
{
    glm::mat4 view = glm::mat4(glm::mat3(camera->getView()));
    glm::mat4 projection = camera->getProjection();
    glm::mat4 MVP = projection * view;
    glUniformMatrix4fv(glGetUniformLocation(this->shader, "MVP"), 1, GL_FALSE, &MVP[0][0]);
}

GLsizei Skybox::getVertexCount()
{
    return 36;
}
void Skybox::unbindBuffers()
{
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
    // enable mask
    glDepthMask(GL_TRUE);
    // unuse shader
    glUseProgram(0);
}

Skybox::~Skybox()
{
    glDeleteBuffers(1, &vao);
}