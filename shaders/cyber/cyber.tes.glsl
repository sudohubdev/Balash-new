#version 450 core

layout(triangles, equal_spacing, cw) in;

in vec3 tcPosition[];   // Input from TCS
in vec3 tcNormal[];     // Input from TCS

out vec3 fPosition;     // Output to the Geometry Shader
out vec3 fNormal;       // Output to the Geometry Shader

uniform float time;    // Time to drive noise

// Function to distort position based on normal and time (zalgo-like)
vec3 applyZalgoDistortion(vec3 pos, vec3 normal, float time) {
    float noiseValue = sin(pos.x * 1.0 + time) * 0.1;  // Simple noise-based distortion
    return pos + noiseValue * normal;
}

void main() {
    vec3 pos = mix(tcPosition[0], tcPosition[1], gl_TessCoord.x) +
               mix(tcPosition[1], tcPosition[2], gl_TessCoord.y);
    
    vec3 normal = normalize(mix(tcNormal[0], tcNormal[1], gl_TessCoord.x));

    // Apply Zalgo-like distortion to the position
    fPosition = applyZalgoDistortion(pos, normal, time + gl_TessCoord.x);
    fNormal = normal;

    gl_Position = vec4(fPosition, 1.0);
}
