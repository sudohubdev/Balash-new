#include "libs.hpp"
#include "loaders/loaders.hpp"
#include "utils/controls.hpp"
#include "utils/skybox.hpp"
#include "objects/lightcube.hpp"
#include "engine.hpp"
#include "objects/NURBS.hpp"
#include "objects/anim/AnimMesh.hpp"
#include "objects/anim/Animation.hpp"
#include "objects/anim/Animator.hpp"
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

    Texture *texture = new Texture("assets/uvgrid.png");
    Geometry *geometry = new Geometry("assets/Model1.obj");
    Mesh *mesh = new Mesh(texture, geometry);
    mesh->scale = glm::vec3(10, 10, 10);

    AnimMesh *mesh2 = new AnimMesh("assets/dancing_vampire.dae");
    mesh2 = mesh2->getChildren()[0];
    mesh2->setTexture(new Texture("assets/Vampire_diffuse.png"));
    mesh2->rotation = glm::vec3(0, 0, 0);
    mesh2->scale = glm::vec3(3, 3, 3);
    mesh2->position = glm::vec3(0, 0, 10);
    Animation dance = Animation("assets/dancing_vampire.dae",mesh2);
    Animator animator(&dance);

    //custom curves
    Texture *uwu = new Texture("assets/uwu.png");
    Texture *uvtest = new Texture("assets/uvgrid.png");
  
    scene.addMesh(mesh);
    scene.addMesh(mesh2);
    scene.addMesh(&skybox);
    glDisable(GL_CULL_FACE);
    mesh->moveRelative(glm::vec3(0, 0, -10));
    mesh2->position = glm::vec3(0, 0, 0);
    // render
    float integal = 0;
    while (!renderer.shouldClose())
    {
        // clear returns time delta
        float tick = renderer.Clear() * 100;
        if (tick != tick || tick > 10 || tick < 0)
            tick = 0;

        integal += tick;
        animator.UpdateAnimation(tick*0.01);
        //animation matrix load
        auto transforms = animator.GetFinalBoneMatrices();
        mesh2->transforms = transforms;
        //cout << endl;
        // circle move cube around dino
        //mesh2->rotation.y += 0.1f;
        //mesh2->moveRelative(glm::vec3(0, 0, tick * 0.1f));
        camera.position += controls.getVelocity() * (0.4f * tick);
        renderer.Render(&scene, &camera);
        // cout << camera.position.x << ", " << camera.position.y << ", " << camera.position.z << endl;
    }
}
