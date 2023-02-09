#version 330 core
in vec3 fragmentColor;
layout(location = 2) in vec2 vertexNormal;
in vec2 UV;
out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;
void main(){
  color = texture( myTextureSampler, UV ).rgb;
  //color.r=1.0;//for debug
  //color.r=UV.x;//for debug
  //color.g=UV.y;//for debug
}
