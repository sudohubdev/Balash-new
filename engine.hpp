#include "libs.hpp"
#include "loaders/loaders.hpp"
using namespace std;

#ifndef ENGINE_HPP
#define ENGINE_HPP

// forward declarations
class Renderer;
class Scene;
class Camera;
class Mesh;
class Texture;
class Geometry;

class Renderer
{
public:
    Renderer();
    ~Renderer();
    double Clear();
    void Render(Scene *scene, Camera *camera);
    bool shouldClose();
    GLFWwindow *getWindow();

protected:
    GLFWwindow *win;
    int resx = 1024;
    int resy = 768;
    GLuint VertexArrayID;
    GLuint programID;
    chrono::high_resolution_clock::time_point lastTime;
    std::vector<float> fps_avg;
};

class Camera
{
public:
    Camera(float fov, float aspect, float near, float far);
    ~Camera();
    void setView(glm::mat4 View);
    void updateProjection();
    void updateView();
    glm::mat4 getView();
    glm::mat4 getProjection();
    void lootAt(glm::vec3 target);
    // gets direction of camera to look at
    glm::vec3 getDirection();

    // camera position
    glm::vec3 position{0, 0, 0};
    // euler angles (radian)
    glm::vec3 rotation{0, 0, 0};
    float fov{75};
    float aspect{1};
    float near{0.1};
    float far{100};

protected:
    glm::mat4 Projection;
    glm::mat4 View;
};

class Scene
{
public:
    Scene();
    ~Scene();
    void addMesh(Mesh *mesh);
    vector<Mesh *> meshes;
};

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
    virtual void setMVP(GLuint shaderID, glm::mat4 MVP);
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
};

class Texture
{
public:
    Texture(const char *path);
    ~Texture();
    GLuint getTextureID();

protected:
    GLuint textureID;
};

class Geometry
{
public:
    Geometry(const char *path);
    Geometry(vector<glm::vec3> vertices, vector<glm::vec2> uvs, vector<glm::vec3> normals);
    Geometry(vector<glm::vec3> vertices);
    ~Geometry();

    vector<glm::vec3> vertices;
    vector<glm::vec2> uvs;
    vector<glm::vec3> normals;
};

#endif