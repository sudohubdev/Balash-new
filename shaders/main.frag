#version 330 core
//in vec3 fragmentColor;
in vec2 UV;
in vec3 Normal;
in vec3 FragPos; 

out vec3 color;
uniform float time;
uniform vec3 camPos;
// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;
uniform samplerCube skybox;
void main(){
    //vec3 I = normalize(FragPos - camPos);
    //vec3 R = reflect(I, normalize(Normal));
    //vec3 colorreflect = texture(skybox, R).rgb;

    //float ratio = 1.00 / 1.52;
    //I = normalize(FragPos - camPos);
    //R = refract(I, normalize(Normal), ratio);
    //vec3 colorrefract = texture(skybox, R).rgb;


    vec2 resolution = vec2(800, 600); // Resolution of the screen
    vec2 uv = UV; // Normalize UV coordinates
    // Add time to create animation
    float distortion = sin(uv.y * 30.0 + time * 10.0) * 0.02;
    // Add stripes artifacts
    float stripes = floor(mod(uv.y * 20.0, 2.0)) * 0.05;
    // Apply distortion and stripes to the UV coordinates
    vec2 distortedUV = vec2(uv.x , uv.y );

    vec3 lightPos = vec3(sin(mod(time * 2.0, 3.14159*1.0))*4.0, 4.0, 0.0);
    vec3 lightColor = vec3(1.0, 1.0, 0.8);
    vec3 objectColor = texture( myTextureSampler, distortedUV ).rgb;


    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);  
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    vec3 specular = vec3(0.0, 0.0, 0.0);
    if (diff > 0.0) {
    vec3 viewDir = normalize(-FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    specular = spec * lightColor;
    }
    vec3 result = (diffuse+specular) * objectColor;
    color = result;//objectColor;

}

