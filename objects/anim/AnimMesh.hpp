#include "../../libs.hpp"
#include "../../engine.hpp"

#ifndef ANIMMESH_HPP
#define ANIMMESH_HPP

class AnimMesh : public Mesh
{
public:
    AnimMesh();
    AnimMesh(string path);
    ~AnimMesh();
    void genBuffers();
    void bindBuffers();
    void unbindBuffers();
    GLuint getVAO();
    void attachShader(GLuint shaderID);
    GLuint getShader();
    void setMVP(GLuint shaderID, glm::mat4 MVP);
    void setMVP(Camera *camera);
    GLsizei getVertexCount();
    AnimMesh *getParent();
    AnimMesh *setParent(AnimMesh *parent);
    std::vector<AnimMesh*> getChildren();
    AnimMesh *addChild(AnimMesh *child);
    void draw(Camera *camera);
private:
    std::vector<AnimMesh*> children;
    AnimMesh *parent;
};
#endif

/*
class Mesh
{
public:
    Mesh(Texture *texture, Geometry *geometry);
    Mesh();
    ~Mesh();
    void setTexture(Texture *texture);
    void setGeometry(Geometry *geometry);
    Texture *getTexture();
    Geometry *getGeometry();
    virtual void genBuffers();
    virtual void bindBuffers();
    GLuint getVAO();
    virtual void unbindBuffers();
    // may be used for particular mesh rendering
    virtual void attachShader(GLuint shaderID);
    virtual GLuint getShader();

    virtual void setMVP(GLuint shaderID, glm::mat4 MVP);
    virtual void setMVP(Camera *camera);
    virtual GLsizei getVertexCount();
    // basic mesh properties
    glm::vec3 position{0, 0, 0};
    glm::vec3 rotation{0, 0, 0};
    glm::vec3 scale{1, 1, 1};
    // moves mesh relative to its current position.
    // unlike position+=direction, this will move the mesh in the direction it is looking at
    void moveRelative(glm::vec3 move);
    glm::mat4 getModelMatrix();

protected:
    Texture *texture;
    Geometry *geometry;
    GLuint vertexbuffer;
    GLuint uvbuffer;
    GLuint normalbuffer;
    GLuint vao;
    GLuint shader;
};
*/