#include "libs.hpp"
#include "loaders/loaders.hpp"
#include "utils/controls.hpp"
#include "engine.hpp"

int main()
{
    // load
    Renderer renderer = Renderer();
    Camera camera = Camera(75.0f, 1.0f, 0.1f, 100.0f);
    camera.position = glm::vec3(3, 4, 6);
    camera.lootAt(glm::vec3(0, 0, 0));

    PointerlockControls controls = PointerlockControls(renderer.getWindow(), &camera);
    controls.Lock();

    Scene scene = Scene();
    Texture *texture = new Texture("dino.png");
    Geometry *geometry = new Geometry("dino.obj");
    Texture *texture2 = new Texture("uvtest.png");
    Geometry *geometry2 = new Geometry("cube.obj");
    Mesh *mesh = new Mesh(texture, geometry);
    Mesh *mesh2 = new Mesh(texture2, geometry2);
    Mesh *mesh3 = new Mesh(texture2, geometry2);
    scene.addMesh(mesh);
    scene.addMesh(mesh2);
    scene.addMesh(mesh3);
    mesh2->position = glm::vec3(-3, 0, 1);
    // render
    while (!renderer.shouldClose())
    {
        double tick = glfwGetTime();
        // mesh->rotation.y += 0.01f;
        mesh2->position.x = cos(tick) * 20;
        mesh2->position.z = sin(tick) * 20;
        mesh3->position.x = -cos(tick) * 20;
        mesh3->position.z = -sin(tick) * 20;
        renderer.Render(&scene, &camera);
        camera.position += controls.getVelocity() * 0.5f;
        cout << camera.position.x << ", " << camera.position.y << ", " << camera.position.z << endl;
    }
}
