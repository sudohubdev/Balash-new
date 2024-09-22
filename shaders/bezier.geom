#version 430 core

layout(lines) in;
layout(line_strip, max_vertices = 4) out;

in vec3 fragColor[];  // Input color from TES
out vec3 outColor;    // Color output to fragment shader


void main() {
    vec4 curvePoint = gl_in[0].gl_Position;  // Bezier curve point
    vec3 tangent = normalize(gl_in[1].gl_Position.xyz - curvePoint.xyz);  // Tangent vector
    // Assuming an up vector (0, 1, 0) for simplicity
    vec3 up = vec3(1.0, 0.0, 0.0);

    // Calculate the normal vector using cross product
    vec3 normal = normalize(cross(tangent, curvePoint.xyz));

    // If the tangent is parallel to the up vector, use another vector
    if (length(normal) < 0.001) {
        normal = normalize(cross(tangent, vec3(0.0, 1.0, 0.0)));
    }


    // Emit the Bezier curve point
    gl_Position = curvePoint;
    outColor = vec3(0.0,1.0,0.0);  // Blue color
    EmitVertex();

    // Emit the second point of the tangent line
    gl_Position = vec4(curvePoint.xyz + tangent * 0.2, 1.0);
    outColor = vec3(0.0,1.0,0.0);  // Blue color
    EmitVertex();

    EndPrimitive();

    
    // Emit the Bezier curve point
    gl_Position = curvePoint;
    outColor = vec3(0.0,0.0,1.0);  // Blue color
    EmitVertex();

    // Emit the second point of the tangent line
    gl_Position = vec4(curvePoint.xyz + normal * 0.2, 1.0);
    outColor = fragColor[0];  // Blue color
    EmitVertex();
    EndPrimitive();
}
