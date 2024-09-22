#version 430 core
layout(vertices = 4) out;  // Cubic Bezier, 4 control points

void main() {
    // Pass the control points to the next shader stage
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    
    gl_TessLevelOuter[0] = 100;
    gl_TessLevelOuter[1] = 100;
}
