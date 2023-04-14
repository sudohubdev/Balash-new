#include "libs.hpp"
#include "loaders/loaders.hpp"
#include "utils/controls.hpp"
#include "utils/skybox.hpp"
#include "objects/lightcube.hpp"
#include "engine.hpp"
#include "objects/NURBS.hpp"

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
    Texture *texture2 = new Texture("assets/uvtest.png");
    Geometry *geometry2 = new Geometry("assets/cube.obj");
    Texture *texture3 = new Texture("assets/girl.png");
    Geometry *geometry3 = new Geometry("assets/girl.obj");
    Mesh *mesh = new Mesh(texture, geometry);
    mesh->scale = glm::vec3(10, 10, 10);
    Mesh *mesh2 = new Mesh(texture2, geometry2);
    Mesh *mesh3 = new Mesh(texture3, geometry3);
    mesh3->scale = glm::vec3(10, 10, 10);

    //custom curves
    Texture *uwu = new Texture("assets/uwu.png");
    Texture *uvtest = new Texture("assets/uvgrid.png");
    Geometry *nurbs = new NURBS({
    glm::vec4(0.3871, 0.1353, 0, 0),
    glm::vec4(0.3333, 0.1688, 0, 0),
    glm::vec4(0.4264, 0.3755, 0, 0),
    glm::vec4(0.4032, 0.4017, 0, 0),
    glm::vec4(0.2270, 0.2576, 0, 0),
    glm::vec4(0.1368, 0.3173, 0, 0),
    glm::vec4(0.1484, 0.4308, 0, 0),
    glm::vec4(0.4017, 0.5021, 0, 0),
    glm::vec4(0.4192, 0.5473, 0, 0),
    glm::vec4(0.1804, 0.6666, 0, 0),
    glm::vec4(0.1994, 0.7772, 0, 0),
    glm::vec4(0.2751, 0.8253, 0, 0),
    glm::vec4(0.4570, 0.5895, 0.1, 0),
    glm::vec4(0.5225, 0.5866, 0.1, 0),
    glm::vec4(0.6186, 0.8238, 0.1, 0),
    glm::vec4(0.7161, 0.8355, 0.1, 0),
    glm::vec4(0.7933, 0.7161, 0.1, 0),
    glm::vec4(0.5691, 0.5312, 0.2, 0),
    glm::vec4(0.5953, 0.4788, 0.2, 0),
    glm::vec4(0.8427, 0.4483, 0.2, 0),
    glm::vec4(0.8340, 0.3114, 0.2, 0),
    glm::vec4(0.7510, 0.2576, 0.2, 0),
    glm::vec4(0.5496, 0.4013, 0.2, 0),
    glm::vec4(0.5196, 0.3813, 0.2, 0),
    glm::vec4(0.5065, 0.1280, 0.2, 0),
    glm::vec4(0.3901, 0.1368, 0.2, 0),
    glm::vec4(0.3871, 0.1353, 0.2, 0)
    }, 50);
    //nurbs->vertices.push_back(glm::vec3(0, 0, 0));//tracer 
    Mesh *mesh4 = new Mesh(uvtest, nurbs);
    mesh4->position = glm::vec3(30, 5, 0);
    mesh4->scale = glm::vec3(30, 30, 30);
    LightCube *lightcube = new LightCube();
    scene.addMesh(mesh);
    scene.addMesh(mesh2);
    scene.addMesh(&skybox);
    scene.addMesh(mesh3);
    scene.addMesh(mesh4);
    glDisable(GL_CULL_FACE);
    // scene.addMesh(lightcube);
    lightcube->attachShader(LoadShaders("shaders/main.vert", "shaders/lightcube.frag"));
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
        mesh4->rotation.y += tick * 0.01f;
        renderer.Render(&scene, &camera);
        // cout << camera.position.x << ", " << camera.position.y << ", " << camera.position.z << endl;
    }
}
