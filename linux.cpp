#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <unistd.h>
#include <iostream>

using namespace glm;

GLFWwindow* win;

int resx=640,resy=480;


void platspec_init(){
    glewExperimental=true;
    
    if(!glfwInit()){
        exit(-1);
    }
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

    
    win=glfwCreateWindow(resx,resy,"balash",NULL,NULL);
    if(!win){
        exit(-2);
    }
    
    glfwMakeContextCurrent(win);
    glewExperimental=true; // Needed in core profile
    
    if(glewInit()!=GLEW_OK){
        exit(-3);
    }
    
    glfwSetInputMode(win, GLFW_STICKY_KEYS, GL_TRUE);
}

void platspec_glclear(){
    glClear(GL_COLOR_BUFFER_BIT);
}

void platspec_term(){
    
    glfwDestroyWindow(win);
    glfwTerminate();
    
}

int platspec_termprog(){
    glfwSwapBuffers(win);
    glfwPollEvents();
    
    return glfwWindowShouldClose(win);
}
