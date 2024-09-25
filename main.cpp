#include "libs.hpp"
#include "loaders/loaders.hpp"
#include "utils/controls.hpp"
#include "utils/skybox.hpp"
#include "objects/lightcube.hpp"
#include "engine.hpp"
#include "objects/NURBS/NURBS.hpp"
#include "objects/anim/AnimMesh.hpp"
#include "objects/anim/Animation.hpp"
#include "objects/anim/Animator.hpp"
#include "objects/tess/tess.hpp"

int main()
{
    // load
    Renderer renderer = Renderer();
    // disable vsync
    // glfwSwapInterval(0);
    Camera camera = Camera(75.0f, 1.0f, 0.1f, 200.0f);
    //ortho camera
    /*
    float w = 1024/100;
    float h = 768/100;
    camera = Camera(w/ -2, w/ 2, h / -2, h /  2, 0.1f, 500.0f);
    */

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
    Mesh *mesh = new Mesh(texture, geometry);
    mesh->debug = true;
    mesh->attachShader(LoadShaders("shaders/cyber/cyber.vert",
                                    "shaders/cyber/cyber.frag",
                                    "shaders/cyber/cyber.geom",
                                    "shaders/cyber/cyber.tcs.glsl",
                                    "shaders/cyber/cyber.tes.glsl"));

    mesh->scale = glm::vec3(10, 10, 10);

    AnimMesh *mesh2 = new AnimMesh("assets/dancing_vampire.dae");
    mesh2 = mesh2->getChildren()[0];
    mesh2->setTexture(new Texture("assets/Vampire_diffuse.png"));
    mesh2->scale = glm::vec3(3, 3, 3);
    mesh2->position = glm::vec3(0, 0, 10);
    Animation dance = Animation("assets/dancing_vampire.dae",mesh2);
    Animator animator(&dance);

    //some other model
    Texture *texture3 = new Texture("assets/dino.png");
    Geometry *geometry3 = new Geometry("assets/dino.obj");
    Mesh *mesh3 = new Mesh(texture3, geometry3);
    mesh3->debug = true;
    mesh3->attachShader(LoadShaders("shaders/cyber/cyber.vert",
                                    "shaders/cyber/cyber.frag",
                                    "shaders/cyber/cyber.geom",
                                    "shaders/cyber/cyber.tcs.glsl",
                                    "shaders/cyber/cyber.tes.glsl"));
    //mesh3->attachShader(LoadShaders("shaders/main.vert", "shaders/maincopy.frag"));
    mesh3->scale = glm::vec3(3, 3, 3);
    mesh3->position = glm::vec3(20, 0, 10);

    //draw circle
    std::vector<glm::vec3> points = std::vector<glm::vec3>();
    std::vector<glm::vec3> normals = std::vector<glm::vec3>();
    std::vector<glm::vec2> uvs = std::vector<glm::vec2>();
    for (float i = 0; i < 360; i += 10)
    {
        float ii = glm::radians(i);
        float prev_ii = glm::radians(i - 10);
        auto currPoint = glm::vec3(cos(ii) * 10, sin(ii) * 10, 0);
        auto prevPoint = glm::vec3(cos(prev_ii) * 10, sin(prev_ii) * 10, 0);
        auto center = glm::vec3(0, 0, 0);
        points.push_back(currPoint);
        points.push_back(prevPoint);
        points.push_back(center);
        normals.push_back(glm::vec3(0, 0, 1));
        normals.push_back(glm::vec3(0, 1, 1));
        normals.push_back(glm::vec3(1, 1, 1));
        uvs.push_back(glm::vec2(cos(ii) , sin(ii) ));
        uvs.push_back(glm::vec2(cos(prev_ii) , sin(prev_ii) ));
        uvs.push_back(glm::vec2(0.5, 0.5));
    }
    Geometry *geometry2 = new Geometry(points, uvs, normals);
    Mesh *mesh4 = new Mesh(new Texture("assets/uwu.png"), geometry2);
    mesh4->moveRelative(glm::vec3(0, 0, -8));
    mesh4->attachShader(LoadShaders("shaders/main.vert", "shaders/maincopy.frag"));
    scene.addMesh(mesh4);
    
    Geometry *geometry5 = new Geometry("assets/arrow.obj");
    Mesh *mesh5 = new Mesh(new Texture(255,255,0), geometry5);
    mesh5->moveRelative(glm::vec3(-5, 0, -5));
    mesh5->rotation = glm::vec3(3.14159/2, 0, 3.14159/2);
    mesh5->scale = glm::vec3(1, 5, 1);
    mesh5->attachShader(LoadShaders("shaders/main.vert", "shaders/maincopy.frag"));
    scene.addMesh(mesh5);

    Mesh *mesh6 = new Mesh(new Texture(0,0,255), geometry5);
    mesh6->moveRelative(glm::vec3(0, 0, 0));
    mesh6->rotation = glm::vec3(0, 3.14159/2, 3.14159/2);
    mesh6->scale = glm::vec3(1, 5, 1);
    mesh6->attachShader(LoadShaders("shaders/main.vert", "shaders/maincopy.frag"));
    scene.addMesh(mesh6);
    
    std::vector<glm::vec3> pointss = {
      glm::vec3(-0.9f, -0.9f, 0),  
      glm::vec3(-0.5f,  1.0f, 0),  
      glm::vec3( 0.5f,  1.0f, 0),  
      glm::vec3( 0.5f, -0.5f, 0),  

      glm::vec3( 0.5f, -0.5f, 0),  
      glm::vec3(-0.5f, -1.0f, 0),  
      glm::vec3(-0.5f, -0.5f, 0),  
      glm::vec3( 0.5f,  0.5f, 0),  
    };

    Bezier*  b = new Bezier(pointss);
    
  
    scene.addMesh(mesh);
    scene.addMesh(mesh2);
    scene.addMesh(mesh3);
    scene.addMesh(&skybox);
    
    //scene.addMesh(b);
    glDisable(GL_CULL_FACE);
    mesh->moveRelative(glm::vec3(15, 0, -10));
    mesh2->position = glm::vec3(0, 0, 0);

    /*renderer.setGLCallback([]() {
        glClear(GL_DEPTH_BUFFER_BIT);
    });*/
    
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
        //animator2.UpdateAnimation(tick*0.01);
        //animation matrix load
        mesh2->transforms = animator.GetFinalBoneMatrices();
        //mesh3->transforms = animator2.GetFinalBoneMatrices();
        //cout << endl;
        // circle move cube around dino
        //mesh2->rotation.y += 0.1f;
        //mesh2->moveRelative(glm::vec3(0, 0, tick * 0.1f));
        camera.position += controls.getVelocity() * (0.4f * tick);
        renderer.Render(&scene, &camera);
        // cout << camera.position.x << ", " << camera.position.y << ", " << camera.position.z << endl;
    }
}
