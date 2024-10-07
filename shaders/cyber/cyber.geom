#version 450 core

layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

in vec3 fPosition[];
in vec3 fNormal[];

out vec3 fragPosition;
out vec3 fragNormal;

uniform mat4 MVP;
uniform mat4 model;
uniform float time;

void generateZalgoSpikes(vec3 basePosition, vec3 normal) {
    for (int i = 0; i < 3; i++) {
        fragPosition = basePosition + (normal * sin(time + i) * .1f);  // Create spike
        fragNormal = normal;
        gl_Position = vec4(fragPosition, 1.0);
        EmitVertex();
    }
    EndPrimitive();
}

vec3 getNormal(vec3 norm){
    return mat3(transpose(inverse(model))) * norm;
}

float rand(float co){
    return fract(sin(dot(vec2(1.0,co), vec2(12.9898, 78.233))) * 43758.5453);
}


void main() {
    for (int i = 0; i < 3; i++) {
        fragPosition =  vec3(model * vec4(fPosition[i], 1.0));
        fragNormal = getNormal(fNormal[i]);
        gl_Position = MVP * vec4(fPosition[i],1);
        EmitVertex();

        // Generate extra "Zalgo" geometry
        //
    }
    EndPrimitive();

    
    fragPosition = fPosition[0];
    fragNormal = fNormal[0];
    gl_Position = MVP * vec4(fPosition[0],1);
    EmitVertex();
    for (int i = 0; i < 2; i++) {
        fragPosition = gl_in[i].gl_Position.xyz + (getNormal(fNormal[i]) * (sin(time * 1.0 + i) + 0.1) * .3);  // Create spike
        fragNormal = getNormal(fNormal[i]);
        gl_Position = MVP * vec4(fragPosition,1);
        
        EmitVertex();
    }
    EndPrimitive();
    //*/
}
