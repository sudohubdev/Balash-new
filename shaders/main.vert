#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition;//model space
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal;

// Output data for fragment shader
out vec2 UV;
out vec3 Normal;
out vec3 FragPos; 
// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 model;

void main(){

    // Output position of the vertex, in clip space : MVP * position
    gl_Position =  MVP * vec4(vertexPosition,1);

    // UV of the vertex. No special space for this one.
    UV = vertexUV;
    FragPos = vec3(model * vec4(vertexPosition, 1.0));
    Normal = mat3(transpose(inverse(model))) * vertexNormal;
}