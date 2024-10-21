#version 450 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inVelocity;
layout(location = 2) in float inLifeTime;
layout(location = 3) in vec4 inColor;

uniform mat4 MVP;
uniform mat4 model;

out vec3 fragVelocity;
out float fragLifeTime;
out vec4 fragColor;

void main() {
    gl_Position = vec4(inPosition, 1.0);
    fragVelocity = inVelocity;
    fragLifeTime = inLifeTime;
    fragColor = inColor;
}
