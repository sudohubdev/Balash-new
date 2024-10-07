#version 410 core

layout(location = 0) in vec3 vertexPosition;//model space

out vec3 outColor;
void main(){
    outColor = vec3(1.0,1.0,0.0);
    gl_Position = vec4(vertexPosition,1);
}