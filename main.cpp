#include "libs.hpp"
#include "loaders/loaders.hpp"
#include "utils/controls.hpp"
#include "utils/skybox.hpp"
#include "objects/lightcube.hpp"
#include "engine.hpp"

int main()
{
    // load
    Renderer renderer = Renderer();
    // disable vsync
    // glfwSwapInterval(0);
    Camera camera = Camera(75.0f, 1.0f, 0.1f, 200.0f);

    PointerlockControls controls = PointerlockControls(renderer.getWindow(), &camera);
    controls.Lock();

    Scene scene = Scene();

    Skybox skybox = Skybox({
        "assets/skybox/right.png",
        "assets/skybox/left.png",
        "assets/skybox/top.png",
        "assets/skybox/bottom.png",
        "assets/skybox/front.png",
        "assets/skybox/back.png",
    });
    skybox.attachShader(LoadShaders("shaders/sky.vert", "shaders/sky.frag"));

    Texture *texture = new Texture("assets/murlok.png");
    Geometry *geometry = new Geometry("assets/murlok.obj");
    Texture *texture2 = new Texture("assets/girl.png");
    Geometry *geometry2 = new Geometry("assets/girl.obj");
    Texture *texture3 = new Texture("assets/girl.png");
    Geometry *geometry3 = new Geometry("assets/girl.obj");
    Mesh *mesh = new Mesh(texture, geometry);
    mesh->scale = glm::vec3(10, 10, 10);
    Mesh *mesh2 = new Mesh(texture2, geometry2);
    Mesh *mesh3 = new Mesh(texture3, geometry3);
    mesh3->scale = glm::vec3(20, 20, 20);
    Mesh *lightcube = new Mesh(texture2, geometry2);
    lightcube->scale = glm::vec3(0.3, 0.3, 0.3);
    scene.addMesh(mesh);
    scene.addMesh(mesh2);
    scene.addMesh(&skybox);
    scene.addMesh(mesh3);
    scene.addMesh(lightcube);
    lightcube->attachShader(mesh->getShader());
    mesh->moveRelative(glm::vec3(0, 0, -10));
    mesh2->position = glm::vec3(0, 0, 0);
    // render
    while (!renderer.shouldClose())
    {
        // clear returns time delta
        float tick = renderer.Clear() * 100;
        if (tick != tick || tick > 10 || tick < 0)
            tick = 0;

        // circle move cube around dino
        mesh2->rotation.y += tick * 0.01f;
        mesh2->moveRelative(glm::vec3(0, 0, tick * 0.1f));
        camera.position += controls.getVelocity() * (0.4f * tick);
        // set light position
        lightcube->position += controls.getVelocityAux(camera.getDirection()) * (0.2f * tick);
        for (auto meshh : scene.meshes)
        {
            GLuint ID = glGetUniformLocation(meshh->getShader(), "lightPos");
            glUniform3f(ID, lightcube->position.x, lightcube->position.y, lightcube->position.z);
        }

        renderer.Render(&scene, &camera);
        // cout << camera.position.x << ", " << camera.position.y << ", " << camera.position.z << endl;
    }
}
