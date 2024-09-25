#version 450 core

layout(vertices = 3) out;

in vec3 vPosition[];    // Input from the Vertex Shader
in vec3 vNormal[];      // Input from the Vertex Shader

out vec3 tcPosition[];  // Output to the TES
out vec3 tcNormal[];    // Output to the TES

void main() {
    if (gl_InvocationID == 0) {
        gl_TessLevelOuter[0] = 4.0;
        gl_TessLevelOuter[1] = 4.0;
        gl_TessLevelOuter[2] = 4.0;
        gl_TessLevelInner[0] = 4.0;
    }

    // Pass the position and normal to the TES
    tcPosition[gl_InvocationID] = vPosition[gl_InvocationID];
    tcNormal[gl_InvocationID] = vNormal[gl_InvocationID];

    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
