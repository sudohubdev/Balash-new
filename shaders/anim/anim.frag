#version 330 core
//in vec3 fragmentColor;
in vec2 UV;
in vec3 Normal;
in vec3 FragPos; 

out vec3 color;
uniform float time;
uniform vec3 camPos;
// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;
uniform samplerCube skybox;
void main(){
    vec3 objectColor = texture( myTextureSampler, UV ).rgb;
    color = objectColor;
}
