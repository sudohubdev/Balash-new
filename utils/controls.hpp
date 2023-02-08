
#include "../libs.hpp"
#include "../engine.hpp"
using namespace std;

#ifndef CONTROLS_HPP
#define CONTROLS_HPP

class PointerlockControls
{
public:
    PointerlockControls(GLFWwindow *window, Camera *camera);
    ~PointerlockControls();
    void Lock();
    void Unlock();
    void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    void mouseCallback(GLFWwindow *window, double xpos, double ypos);
    // gets velocity of camera
    void resetDelta();
    glm::vec3 getVelocity();
    glm::vec2 getMouseDelta();
    void resize(int width, int height);

protected:
    GLFWwindow *win;
    Camera *camera;
    bool locked = false;
    glm::vec3 velocity;
    glm::vec2 lastMousePos;
};
#endif
