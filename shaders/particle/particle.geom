#version 330 core
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 MVP;
in vec3 fragVelocity[];
in float fragLifeTime[];
in vec4 fragColor[];

out vec4 color;

void main() {
    float size = fragLifeTime[0] * 0.1; // Particle size decreases with lifetime
    vec4 pos = MVP * gl_in[0].gl_Position;

    color = fragColor[0];

    gl_Position = pos + vec4(-size, -size, 0.0, 0.0);
    EmitVertex();

    gl_Position = pos + vec4(size, -size, 0.0, 0.0);
    EmitVertex();

    gl_Position = pos + vec4(-size, size, 0.0, 0.0);
    EmitVertex();

    gl_Position = pos + vec4(size, size, 0.0, 0.0);
    EmitVertex();

    EndPrimitive();
}
