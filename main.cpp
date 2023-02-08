#include "libs.hpp"
#include "loaders/loaders.hpp"
#include "utils/controls.hpp"
#include "utils/skybox.hpp"
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
        "skybox/right.png",
        "skybox/left.png",
        "skybox/top.png",
        "skybox/bottom.png",
        "skybox/front.png",
        "skybox/back.png",
    });
    skybox.attachShader(LoadShaders("skyvert.glsl", "skyfrag.glsl"));

    Texture *texture = new Texture("dino.png");
    Geometry *geometry = new Geometry("dino.obj");
    Texture *texture2 = new Texture("uvtest.png");
    Geometry *geometry2 = new Geometry("cube.obj");
    Mesh *mesh = new Mesh(texture, geometry);
    Mesh *mesh2 = new Mesh(texture2, geometry2);
    Mesh *mesh3 = new Mesh(texture2, geometry2);
    scene.addMesh(mesh);
    scene.addMesh(mesh2);
    scene.addMesh(&skybox);
    scene.addMesh(mesh3);
    mesh2->position = glm::vec3(0, 0, 0);
    // render
    while (!renderer.shouldClose())
    {
        // clear returns time delta
        float tick = renderer.Clear() * 100;
        if (tick != tick || tick > 10 || tick < 0)
            tick = 0;

        // circle move cube around dino
        mesh2->rotation.y += tick * 0.05f;
        mesh2->moveRelative(glm::vec3(0, 0, tick * 0.2f));

        mesh3->rotation.z += tick * 0.1f;
        mesh3->moveRelative(glm::vec3(tick * 0.5f, 0, 0));
        renderer.Render(&scene, &camera);
        camera.position += controls.getVelocity() * (0.4f * tick);
        // cout << camera.position.x << ", " << camera.position.y << ", " << camera.position.z << endl;
    }
}
