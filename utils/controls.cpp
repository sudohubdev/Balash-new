#include "controls.hpp"

PointerlockControls::PointerlockControls(GLFWwindow *window, Camera *camera)
{
    this->win = window;
    this->camera = camera;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetWindowUserPointer(window, this);

    auto keyCallback = [](GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        PointerlockControls *self = static_cast<PointerlockControls *>(glfwGetWindowUserPointer(window));
        self->keyCallback(window, key, scancode, action, mods);
    };
    auto mouseCallback = [](GLFWwindow *window, double xpos, double ypos)
    {
        PointerlockControls *self = static_cast<PointerlockControls *>(glfwGetWindowUserPointer(window));
        self->mouseCallback(window, xpos, ypos);
    };
    auto clickCallback = [](GLFWwindow *window, int button, int action, int mods)
    {
        PointerlockControls *self = static_cast<PointerlockControls *>(glfwGetWindowUserPointer(window));
        // self->clickCallback(window, int button, int action, int mods);
        self->resetDelta();
        self->Lock();
    };
    auto resizeCallback = [](GLFWwindow *window, int width, int height)
    {
        PointerlockControls *self = static_cast<PointerlockControls *>(glfwGetWindowUserPointer(window));
        glViewport(0, 0, width, height);
        self->resize(width, height);
    };

    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetMouseButtonCallback(window, clickCallback);
    glfwSetWindowSizeCallback(window, resizeCallback);

    if (glfwRawMouseMotionSupported())
    {
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }
    // glfwGetCursorPos(window, &lastMousePos.x, &lastMousePos.y);
    velocity = glm::vec3(0, 0, 0);
}

PointerlockControls::~PointerlockControls()
{
    //
}

void PointerlockControls::Lock()
{
    if (locked)
        return;
    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    locked = true;
}

void PointerlockControls::Unlock()
{
    if (!locked)
        return;
    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    locked = false;
}
void PointerlockControls::resetDelta()
{
    double x, y;
    glfwGetCursorPos(win, &x, &y);
    lastMousePos = glm::vec2(x, y);
}
void PointerlockControls::resize(int width, int height)
{
    glViewport(0, 0, width, height);
    this->camera->aspect = (float)width / (float)height;
    this->camera->updateProjection();
}
glm::vec3 PointerlockControls::getVelocity()
{
    // https://github.com/arduinka55055/threejs-minecraft/blob/master/static/src/lib/controlfirst.js
    this->camera->updateView();
    glm::mat4 rot = this->camera->getView();
    glm::vec3 res = glm::vec3(0, 0, 0);
    // forward
    glm::vec3 vec = glm::vec3(rot[0][0], rot[1][0], rot[2][0]);
    vec = glm::cross(vec, glm::vec3(0, 1, 0));
    res -= vec * velocity.z;
    // right
    vec = glm::vec3(rot[0][0], rot[1][0], rot[2][0]);
    res += vec * velocity.x;
    // up
    res.y += velocity.y;

    if (res.x != res.x || res.y != res.y || res.z != res.z)
        return glm::vec3(0, 0, 0);
    return res;
}

void PointerlockControls::mouseCallback(GLFWwindow *window, double xpos, double ypos)
{
    if (!locked)
        return;
    cout << "mouseCallback" << endl;
    glm::vec2 mousePos = glm::vec2(xpos, ypos);
    glm::vec2 mouseDelta = mousePos - lastMousePos;
    lastMousePos = mousePos;
    if (mouseDelta.x != mouseDelta.x || mouseDelta.y != mouseDelta.y)
        return;

    camera->rotation.y -= mouseDelta.y * 0.005f;
    camera->rotation.z += mouseDelta.x * 0.005f;

    // clip rotation y
    if (camera->rotation.y > 3.14f / 2.0f)
        camera->rotation.y = 3.14f / 2.0f;
    if (camera->rotation.y < -3.14f / 2.0f)
        camera->rotation.y = -3.14f / 2.0f;

    camera->rotation.z = glm::mod(camera->rotation.z, 2 * glm::pi<float>());
}
void PointerlockControls::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        this->Unlock();

    if (key == GLFW_KEY_F11 && action == GLFW_PRESS)
    {
        bool fullscreen = glfwGetWindowMonitor(window) != NULL;
        if (fullscreen)
        {
            glfwSetWindowMonitor(window, nullptr, 0, 0, 800, 600, 0);
            return;
        }
        int monitorwidth, monitorheight;
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        glfwGetMonitorPhysicalSize(monitor, &monitorwidth, &monitorheight);
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }

    if (key == GLFW_KEY_W && action == GLFW_PRESS)
        velocity.z += 1;
    if (key == GLFW_KEY_W && action == GLFW_RELEASE)
        velocity.z -= 1;

    if (key == GLFW_KEY_S && action == GLFW_PRESS)
        velocity.z -= 1;
    if (key == GLFW_KEY_S && action == GLFW_RELEASE)
        velocity.z += 1;

    if (key == GLFW_KEY_A && action == GLFW_PRESS)
        velocity.x -= 1;
    if (key == GLFW_KEY_A && action == GLFW_RELEASE)
        velocity.x += 1;

    if (key == GLFW_KEY_D && action == GLFW_PRESS)
        velocity.x += 1;
    if (key == GLFW_KEY_D && action == GLFW_RELEASE)
        velocity.x -= 1;

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        velocity.y += 1;
    if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
        velocity.y -= 1;

    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
        velocity.y -= 1;
    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE)
        velocity.y += 1;

    velocity.x = glm::clamp(velocity.x, -1.0f, 1.0f);
    velocity.y = glm::clamp(velocity.y, -1.0f, 1.0f);
    velocity.z = glm::clamp(velocity.z, -1.0f, 1.0f);
}
