#version 330 core

layout(location = 0) in vec3 vertexPosition;//model space

void main(){
    gl_Position = vec4(vertexPosition,1);
}