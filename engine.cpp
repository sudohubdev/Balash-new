#include "engine.hpp"
using namespace std;

// Renderer
#pragma region "renderer"

static GLuint GLOBALSHADER = 0;

Renderer::Renderer()
{
    // init
    glewExperimental = true;
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW");
        exit(-1);
    }
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
#ifndef __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
#endif
#ifdef __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);
#endif

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL
    // Open a window and create its OpenGL context
    win = glfwCreateWindow(resx, resy, "balash", NULL, NULL);
    if (!win)
    {
        fprintf(stderr, "Failed to open GLFW window");
        exit(-2);
    }
    glfwMakeContextCurrent(win);
    glewExperimental = true; // Needed in core profile
    GLenum glewErr = glewInit();
    if (glewErr != GLEW_OK && glewErr != GLEW_ERROR_NO_GLX_DISPLAY)
    {
        fprintf(stderr, "Error initializing GLEW: %s", glewGetErrorString(glewErr));
        exit(-3);
    }
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(win, GLFW_STICKY_KEYS, GL_TRUE);

    glActiveTexture(GL_TEXTURE);
    // Enabling features
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);
    // Create and compile our GLSL program from the shaders (global)
    programID = LoadShaders("shaders/main.vert", "shaders/main.frag");
    GLOBALSHADER = programID;
}

Renderer::~Renderer()
{
    // Close OpenGL window and terminate GLFW
    glfwDestroyWindow(win);
    glfwTerminate();
}
double Renderer::Clear()
{
    // Swap buffers to prevent flickering
    glfwSwapBuffers(win);
    glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // get FPS
    double diff = chrono::duration_cast<chrono::duration<double>>(chrono::high_resolution_clock::now() - lastTime).count();
    lastTime = chrono::high_resolution_clock::now();
    fps_avg.push_back(1 / diff);
    // remove old values
    if (fps_avg.size() > 20)
        fps_avg.erase(fps_avg.begin());

    vector sorted = fps_avg;
    std::sort(sorted.begin(), sorted.end());
    int size = sorted.size();
    double median = (size % 2 != 0) ? (double)sorted[size / 2] : ((double)sorted[size / 2] + (double)sorted[size / 2 - 1]) / 2;

    // used vram
    int vram = 0;
    int total = 0;
    glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &vram);
    glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &total);
    vram /= 1024;
    total /= 1024;
    vram = total - vram;
    // set title
    string title = "BalashEngine  |  FPS: " + to_string(median) + "  |  VRAM Used(TOTAL): " + to_string(vram) + "MB/" + to_string(total) + "MB";
    glfwSetWindowTitle(win, title.c_str());
    return diff;
}
void Renderer::Render(Scene *scene, Camera *camera)
{
    // Render
    for (Mesh *mesh : scene->meshes)
    {
        // cout << "Rendering mesh" << endl;
        // cout << "Mesh has " << mesh->getGeometry()->vertices.size() << "vertices" << endl;
        // cout << "Mesh texture is " << mesh->getTexture()->getTextureID() << endl;
        //  mesh->bindBuffers();
        mesh->draw(camera);
    }
    if (this->callback != NULL)
        this->callback();
}
void Renderer::setGLCallback(void (*callback)(void))
{
    this->callback = callback;
}
bool Renderer::shouldClose()
{
    return glfwWindowShouldClose(win);
}
GLFWwindow *Renderer::getWindow()
{
    return win;
}

#pragma endregion

// Camera
#pragma region "camera"

Camera::Camera(float fov, float aspect, float near, float far)
{
    this->fov = fov;
    this->aspect = aspect;
    this->near = near;
    this->far = far;
    this->position = glm::vec3(0, 0, 0);
    this->rotation = glm::vec3(0, 0, 0);
    updateProjection();
}
//ortho
Camera::Camera(float left, float right, float bottom, float top, float near, float far)
{
    this->fov = 0;
    this->near = near;
    this->far = far;
    this->ortho = glm::vec4(left, right, bottom, top);
    this->position = glm::vec3(0, 0, 0);
    this->rotation = glm::vec3(0, 0, 0);
    updateProjection();
}
void Camera::updateProjection()
{
    if (fov == 0)
        this->Projection = glm::ortho(ortho.x, ortho.y, ortho.z, ortho.w, near, far);
    else
        this->Projection = glm::perspective(glm::radians(fov), aspect, near, far);
}
void Camera::setView(glm::mat4 View)
{
    throw "Not implemented";
}
glm::mat4 Camera::getView()
{
    return this->View;
}
glm::mat4 Camera::getProjection()
{
    return this->Projection;
}
void Camera::updateView()
{
    this->View = glm::lookAt(
        this->position,
        this->position + this->getDirection(),
        glm::vec3(0, 1, 0) // Head is up (set to 0,-1,0 to look upside-down)
    );
}
void Camera::lootAt(glm::vec3 target)
{
    this->View = glm::lookAt(
        this->position,
        target,
        glm::vec3(0, 1, 0) // Head is up (set to 0,-1,0 to look upside-down)
    );
    // update rotation
    this->rotation = glm::normalize(target - this->position);
}
glm::vec3 Camera::getDirection()
{
    glm::vec3 direction;
    direction.x = cos(this->rotation.z) * cos(this->rotation.y);
    direction.y = sin(this->rotation.y);
    direction.z = sin(this->rotation.z) * cos(this->rotation.y);
    return glm::normalize(direction);
}
Camera::~Camera()
{
}
#pragma endregion

// Scene
#pragma region "scene"
Scene::Scene()
{
}
Scene::~Scene()
{
}
void Scene::addMesh(Mesh *mesh)
{
    meshes.push_back(mesh);
}
#pragma endregion

// Mesh
#pragma region "mesh"

Mesh::Mesh(Texture *texture, Geometry *geometry)
{
    this->texture = texture;
    this->geometry = geometry;
    this->attachShader(GLOBALSHADER);
    this->genBuffers();
}
Mesh::Mesh()
{
    drawtype = 0;
}
Mesh::~Mesh()
{
    // deleting buffers
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &uvbuffer);
    glDeleteBuffers(1, &normalbuffer);
}
void Mesh::setTexture(Texture *texture)
{
    this->texture = texture;
}
void Mesh::setGeometry(Geometry *geometry)
{
    this->geometry = geometry;
}
Texture *Mesh::getTexture()
{
    return this->texture;
}
Geometry *Mesh::getGeometry()
{
    return this->geometry;
}
void Mesh::genBuffers()
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, this->geometry->vertices.size() * sizeof(glm::vec3), this->geometry->vertices.data(), GL_STATIC_DRAW);
    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, this->geometry->uvs.size() * sizeof(glm::vec2), this->geometry->uvs.data(), GL_STATIC_DRAW);
    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, this->geometry->normals.size() * sizeof(glm::vec3), this->geometry->normals.data(), GL_STATIC_DRAW);
}
void Mesh::bindBuffers()
{
    glUseProgram(this->getShader());
    glBindVertexArray(vao);
    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
        0,        // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,        // size vector 3d
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,        // stride
        (void *)0 // array buffer offset
    );
    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(
        1,        // attribute. No particular reason for 1, but must match the layout in the shader.
        2,        // size : vector 2d
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,        // stride
        (void *)0 // array buffer offset
    );
    // 3rd attribute buffer : normals
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(
        2,        // attribute. No particular reason for 2, but must match the layout in the shader.
        3,        // size : vector 3d
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,        // stride
        (void *)0 // array buffer offset
    );
    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);                                // Hey OpenGL, we're about to give commands for texture sampler 0.
    glBindTexture(GL_TEXTURE_2D, this->texture->getTextureID()); // when the currently-active sampler samples from a 2D texture, this is the texture to use.
    glEnable(GL_TEXTURE_2D);                                     // the currently-active sampler should actually use the bound 2D texture.
}
void Mesh::attachShader(GLuint shaderID)
{
    // Use our shader
    this->shader = shaderID;
    // glUseProgram(shaderID);
}
GLuint Mesh::getShader()
{
    return this->shader;
}
void Mesh::unbindBuffers()
{
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    // Disable our texture
    glDisable(GL_TEXTURE_2D);
    // Unbind vao
    glBindVertexArray(0);
}
void Mesh::setMVP(GLuint shaderID, glm::mat4 MVP)
{
    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    GLuint MatrixID = glGetUniformLocation(shaderID, "MVP");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
}
// does almost everything needed with matrices
void Mesh::setMVP(Camera *camera)
{
    camera->updateProjection();
    camera->updateView();
    glm::mat4 MVP = camera->getProjection() * camera->getView() * this->getModelMatrix();
    this->setMVP(this->shader, MVP);
    GLuint MatrixID = glGetUniformLocation(this->getShader(), "model");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &this->getModelMatrix()[0][0]);
}

GLsizei Mesh::getVertexCount()
{
    return this->geometry->vertices.size();
}
void Mesh::draw(Camera *camera){
    bindBuffers();
    setMVP(camera);
    // set uniforms
    GLuint ID = glGetUniformLocation(getShader(), "time");
    glUniform1f(ID, (float)glfwGetTime());
    ID = glGetUniformLocation(getShader(), "camPos");
    glUniform3f(ID, camera->position.x, camera->position.y, camera->position.z);
    if(debug){
        glDrawArrays(GL_PATCHES, 0, getVertexCount());
    }
    else {
        if(drawtype == 0)
            glDrawArrays(GL_TRIANGLES, 0, getVertexCount());
        else
            glDrawArrays(drawtype, 0, getVertexCount());
    }
    unbindBuffers();
}
void Mesh::moveRelative(glm::vec3 move)
{
    glm::mat4 rot = glm::mat3(1.0f);
    rot = glm::rotate(rot, rotation.x, glm::vec3(1, 0, 0));
    rot = glm::rotate(rot, rotation.y, glm::vec3(0, 1, 0));
    rot = glm::rotate(rot, rotation.z, glm::vec3(0, 0, 1));
    glm::mat3 rot3 = glm::mat3(rot);
    position += rot3 * move;
}

glm::mat4 Mesh::getModelMatrix()
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, rotation.x, glm::vec3(1, 0, 0));
    model = glm::rotate(model, rotation.y, glm::vec3(0, 1, 0));
    model = glm::rotate(model, rotation.z, glm::vec3(0, 0, 1));
    model = glm::scale(model, scale);
    return model;
}
#pragma endregion

// Texture
#pragma region "texture"

Texture::Texture()
{
    // cout << "Texture loaded from memory. beware nulls!" << endl;
    textureID = 0;
}
Texture::Texture(unsigned char r, unsigned char g, unsigned char b)
{
    unsigned char rgb[3] = {r, g, b};
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}
Texture::Texture(const char *path)
{
    cout << "Texture loaded from " << path << endl;
    assert(sizeof(path) > 0);
    string s = path;
    assert(s.find("png") != string::npos);
    textureID = loadpng(path);
    // assert(textureID != 0);
}
Texture::~Texture()
{
    // deleting texture:
    glDeleteTextures(1, &textureID);
}
GLuint Texture::getTextureID()
{
    return textureID;
}
#pragma endregion

// Geometry
#pragma region "geometry"

Geometry::Geometry()
{
    this->vertices = vector<glm::vec3>();
    this->uvs = vector<glm::vec2>();
    this->normals = vector<glm::vec3>();
    this->indices = vector<unsigned int>();
    // cout << "Geometry loaded from memory. beware of nulls!" << endl;
}

Geometry::Geometry(const char *path)
{
    cout << "Geometry loaded from " << path << endl;
    assert(sizeof(path) > 0);
    string s = path;
    assert(s.find("obj") != string::npos);
    bool res = loadOBJ(path, vertices, uvs, normals);
    assert(res);
}
Geometry::Geometry(vector<glm::vec3> vertices, vector<glm::vec2> uvs, vector<glm::vec3> normals)
{
    this->vertices = vertices;
    this->uvs = uvs;
    this->normals = normals;
}
Geometry::Geometry(vector<glm::vec3> vertices)
{
    this->vertices = vertices;
}
Geometry::~Geometry()
{
}
#pragma endregion