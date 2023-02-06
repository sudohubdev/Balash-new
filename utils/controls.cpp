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

    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
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
    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    locked = true;
}

void PointerlockControls::Unlock()
{
    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    locked = false;
}

glm::vec3 PointerlockControls::getVelocity()
{
    this->camera->updateView();
    glm::mat4 rot = this->camera->getView();
    // Calculate axises
    glm::vec3 cameraFront = glm::vec3(rot[2][0], rot[2][1], rot[2][2]);
    glm::vec3 cameraUp = glm::vec3(rot[1][0], rot[1][1], rot[1][2]);
    glm::vec3 cameraRight = glm::vec3(rot[0][0], rot[0][1], rot[0][2]);
    // Calculate velocity in global space

    glm::vec3 res = glm::vec3(0, 0, 0);
    res.x = cameraRight.x * velocity.x + cameraFront.x * velocity.z;
    res.y = velocity.y;
    res.z = -cameraRight.z * velocity.x - cameraFront.z * velocity.z;

    if (res.x != res.x || res.y != res.y || res.z != res.z)
        return glm::vec3(0, 0, 0);
    return res;
}

void PointerlockControls::mouseCallback(GLFWwindow *window, double xpos, double ypos)
{
    // if (!locked)
    // return;
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
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_W && action == GLFW_PRESS)
        velocity.z = 1;
    else if (key == GLFW_KEY_W && action == GLFW_RELEASE)
        velocity.z = 0;

    if (key == GLFW_KEY_S && action == GLFW_PRESS)
        velocity.z = -1;
    else if (key == GLFW_KEY_S && action == GLFW_RELEASE)
        velocity.z = 0;

    if (key == GLFW_KEY_A && action == GLFW_PRESS)
        velocity.x = -1;
    else if (key == GLFW_KEY_A && action == GLFW_RELEASE)
        velocity.x = 0;

    if (key == GLFW_KEY_D && action == GLFW_PRESS)
        velocity.x = 1;
    else if (key == GLFW_KEY_D && action == GLFW_RELEASE)
        velocity.x = 0;

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        velocity.y = 1;
    else if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
        velocity.y = 0;

    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
        velocity.y = -1;
    else if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE)
        velocity.y = 0;
}
