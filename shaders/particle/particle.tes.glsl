#version 450 core

layout(quads, fractional_odd_spacing) in;

in vec3 tcsParticlePos[];
in vec4 tcsParticleColor[];

out vec2 texCoord;
out vec4 evalColor;

uniform mat4 viewProjMatrix;
uniform vec3 cameraRight;
uniform vec3 cameraUp;

void main() {
    vec3 particlePos = tcsParticlePos[0];

    // Generate quad points for the particle, using cameraRight and cameraUp for billboard orientation
    vec3 offset = (gl_TessCoord.x - 0.5) * 2.0 * cameraRight + (gl_TessCoord.y - 0.5) * 2.0 * cameraUp;
    vec3 worldPos = particlePos + offset;

    // Project the position into screen space
    gl_Position = viewProjMatrix * vec4(worldPos, 1.0);

    // Set the UV coordinates for texturing
    texCoord = vec2(gl_TessCoord.x, gl_TessCoord.y);
    evalColor = tcsParticleColor[0]; // Pass color to fragment shader
}
