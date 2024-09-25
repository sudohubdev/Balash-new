#version 450 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 inNormal;

out vec3 vPosition;
out vec3 vNormal;

uniform mat4 MVP;
uniform mat4 model;

void main() {
    vPosition = inPosition;
    vNormal = inNormal;
    gl_Position = vec4(inPosition, 1.0);
/*
    fPosition =  vec3(model * vec4(inPosition, 1.0));
    fNormal = mat3(transpose(inverse(model))) * inNormal;
    gl_Position = MVP * vec4(inPosition,1);
    //*/
}
