#version 330 core
in vec3 fragmentColor;
in vec2 UV;
out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;
void main(){
  color = texture( myTextureSampler, UV ).rgb;
  color.r=1.0;//for debug
  //color.r=UV.x;//for debug
  //color.g=UV.y;//for debug
  //uv loads, coords too
  //but texture sampler is a dick
  //check linux.cpp
}
