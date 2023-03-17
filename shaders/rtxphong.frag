#version 460

layout(binding = 0, rgba32f) uniform image2D resultImage;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 color;
};

struct RayPayload {
    vec3 color;
    float t;
};

struct Ray {
    vec3 origin;
    vec3 direction;
};

uniform Material material;
uniform Light light;

vec3 reflect(vec3 I, vec3 N) {
    return I - 2.0 * dot(I, N) * N;
}

bool intersectSphere(vec3 center, float radius, Ray ray, out float t) {
    vec3 oc = ray.origin - center;
    float a = dot(ray.direction, ray.direction);
    float b = 2.0 * dot(oc, ray.direction);
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b * b - 4.0 * a * c;
    if (discriminant < 0.0) {
        return false;
    } else {
        float t1 = (-b - sqrt(discriminant)) / (2.0 * a);
        float t2 = (-b + sqrt(discriminant)) / (2.0 * a);
        if (t1 < 0.0 && t2 < 0.0) {
            return false;
        } else if (t1 >= 0.0 && t2 >= 0.0) {
            t = min(t1, t2);
        } else {
            t = max(t1, t2);
        }
        return true;
    }
}

void trace(Ray ray, inout RayPayload payload) {
    float t;
    if (intersectSphere(vec3(0.0, 0.0, 0.0), 1.0, ray, t)) {
        vec3 P = ray.origin + t * ray.direction;
        vec3 N = normalize(P - vec3(0.0, 0.0, 0.0));
        vec3 L = normalize(light.position - P);
        vec3 V = normalize(-ray.direction);
        vec3 R = reflect(-L, N);

        vec3 ambient = material.ambient;
        vec3 diffuse = material.diffuse * light.color * max(dot(N, L), 0.0);
        vec3 specular = material.specular * light.color * pow(max(dot(R, V), 0.0), material.shininess);

        payload.color = ambient + diffuse + specular;
        payload.t = t;
    } else {
        payload.color = vec3(0.0, 0.0, 0.0);
        payload.t = -1.0;
    }
}

void main() {
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
    vec2 pixelSize = 1.0 / vec2(imageSize(resultImage).xy);
    vec2 uv = (vec2(pixelCoords) + 0.5) * pixelSize;
    uv.y = 1.0 - uv.y;

    Ray ray;
    ray.origin = vec3(0.0, 0.0, -5.0);
    ray.direction = normalize(vec3(uv, 1.0));

    RayPayload payload;
    trace(ray, payload);

    imageStore(resultImage, pixelCoords, vec4(payload.color, 1.0));
}
