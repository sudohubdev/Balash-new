#version 430 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition;//model space
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in ivec4 boneIds; 
layout(location = 4) in vec4 weights;

// Output data for fragment shader
out vec2 UV;
out vec3 Normal;
out vec3 FragPos; 
out vec4 debug;
// Values that stay constant for the whole mesh.
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];
void main(){

    vec4 totalPosition = vec4(0.0f);
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(boneIds[i] == -1) 
            continue;
        if(boneIds[i] >=MAX_BONES) 
        {
            totalPosition = vec4(vertexPosition,1.0f);
            break;
        }
        vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(vertexPosition,1.0f);
        totalPosition += localPosition * weights[i];
    }
    debug = totalPosition * vec4(1.0f);
    mat4 viewModel = view * model;
    gl_Position = projection * viewModel * totalPosition;
    FragPos = vec3(model * totalPosition);
    UV = vertexUV;
} 