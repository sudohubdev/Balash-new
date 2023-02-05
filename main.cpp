#include "libs.hpp"
#include "loaders/loaders.hpp"
#include "engine.hpp"

int main()
{
    // load
    Renderer renderer = Renderer();
    Camera camera = Camera(75.0f, 1.0f, 0.1f, 100.0f);
    camera.position = glm::vec3(3, 4, 6);
    camera.lootAt(glm::vec3(0, 0, 0));
    Scene scene = Scene();
    Texture *texture = new Texture("dino.png");
    Geometry *geometry = new Geometry("dino.obj");
    Texture *texture2 = new Texture("uvtest.png");
    Geometry *geometry2 = new Geometry("cube.obj");
    Mesh *mesh = new Mesh(texture, geometry);
    Mesh *mesh2 = new Mesh(texture2, geometry2);
    scene.addMesh(mesh);
    scene.addMesh(mesh2);
    mesh2->position = glm::vec3(-3, 0, 1);
    // render
    while (!renderer.shouldClose())
    {
        double tick = glfwGetTime();
        mesh->rotation.y += 0.01f;
        mesh2->position.x = cos(tick) * 3;
        mesh2->position.z = sin(tick) * 3;
        renderer.Render(&scene, &camera);
    }
}
