#version 410 core

in vec3 outColor;    // Color from geometry shader

out vec4 fragColor;  // Final color output

void main() {
    fragColor = vec4(outColor, 1.0);  // Output the color (RGB)
}
