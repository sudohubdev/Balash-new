#version 400 core

layout(isolines, equal_spacing, cw) in;


out vec3 fragColor;       // Output color

// Bezier curve interpolation
vec3 bezier(float t, vec3 p0, vec3 p1, vec3 p2, vec3 p3) {
    float u = 1.0 - t;
    return u*u*u*p0 + 3.0*u*u*t*p1 + 3.0*u*t*t*p2 + t*t*t*p3;
}

// Tangent calculation for cubic Bezier curve
vec3 bezierTangent(float t, vec3 p0, vec3 p1, vec3 p2, vec3 p3) {
    float u = 1.0 - t;
    return 3.0 * u * u * (p1 - p0) + 6.0 * u * t * (p2 - p1) + 3.0 * t * t * (p3 - p2);
}

void main() {
    float t = gl_TessCoord.x;  // Parametric coordinate for isolines

    vec3 p0 = gl_in[0].gl_Position.xyz;
    vec3 p1 = gl_in[1].gl_Position.xyz;
    vec3 p2 = gl_in[2].gl_Position.xyz;
    vec3 p3 = gl_in[3].gl_Position.xyz;

    // Calculate position on the Bezier curve
    vec3 position = bezier(t, p0, p1, p2, p3);

    // Calculate the tangent vector at this point
    vec3 tangent = normalize(bezierTangent(t, p0, p1, p2, p3));

    // Pass position to the next stage (fragment shader)
    gl_Position = vec4(position, 1.0);

    fragColor = vec3(1.0,0.0,0.0);  // red
}
