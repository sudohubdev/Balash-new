#version 450 core

layout(vertices = 1) out; // We're using a point-particle system, so we only need one vertex per particle

in vec3 tessParticlePos[];
in vec4 tessParticleColor[];

out vec3 tcsParticlePos[];
out vec4 tcsParticleColor[];

uniform float tessLevel = 4.0;  // Level of tessellation

void main() {
    // Pass through particle data
    tcsParticlePos[gl_InvocationID] = tessParticlePos[gl_InvocationID];
    tcsParticleColor[gl_InvocationID] = tessParticleColor[gl_InvocationID];

    // Set tessellation levels (outer and inner)
    gl_TessLevelOuter[0] = tessLevel;  // Number of subdivisions for the quad
    gl_TessLevelOuter[1] = tessLevel;
    gl_TessLevelOuter[2] = tessLevel;
    gl_TessLevelOuter[3] = tessLevel;
    gl_TessLevelInner[0] = tessLevel;  // For triangle tessellation (optional if desired)
}
