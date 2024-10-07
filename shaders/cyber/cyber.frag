#version 450 core

in vec3 fragPosition;
in vec3 fragNormal;

out vec4 FragColor;

uniform float time;

void main() {
    float noiseEffect = sin(fragPosition.x * 5.0 + time) * 0.5 + 0.5;
    vec3 color = cross(fragPosition, fragNormal * noiseEffect) * 5.0;  // Glitchy color
    FragColor = vec4(color, 1.0);
}
