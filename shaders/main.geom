

#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 140) out;

in VS_OUT {
    vec3 color;
} gs_in[];

out vec3 fColor;

#define size 0.2
#define PI 3.1415926535897932384626433832795

void makesquare(){
    vec2 center = vec2(-0.6, 0.6);
    vec2 A = vec2(center.x - size, center.y - size);
    vec2 B = vec2(center.x + size, center.y - size);
    vec2 C = vec2(center.x + size, center.y + size);
    vec2 D = vec2(center.x - size, center.y + size);

    fColor = vec3(0.5, 0.5, 0.0);

    gl_Position = vec4(A, 0.0, 1.0); 
    EmitVertex();

    gl_Position = vec4(B, 0.0, 1.0);
    EmitVertex();

    gl_Position = vec4(D, 0.0, 1.0);
    EmitVertex();

    gl_Position = vec4(C, 0.0, 1.0);
    EmitVertex();

    EndPrimitive();
}

//x is 0.5 + 0.5 * x
void maketriangle(){
    fColor = vec3(0.5, 1.0, 0.0);
    gl_Position = vec4(0.3, 0.3, 0.0, 1.0);
    EmitVertex();
    gl_Position = vec4(0.7, 0.3, 0.0, 1.0);
    EmitVertex();
    gl_Position = vec4(0.5, 0.7, 0.0, 1.0);
    EmitVertex();

    EndPrimitive();
}
void makecircle(){
    fColor = vec3(0.5, 0.0, 1.0);
    float steps = 20.0;
    float segment = 2.0 * PI / steps;
    //use triangle strip to draw a circle
    gl_Position = vec4(-0.5, -0.5, 0.0, 1.0);
    EmitVertex();
    for (int i = 0; i <= steps; i++) {
        float angle = i * segment;
        float aspect = 4.0 / 3.0;//aspect ratio of the window
        //current position
        gl_Position = vec4(-0.5 + 0.2 * cos(angle), -0.5 + 0.2 * sin(angle) * aspect, 0.0, 1.0);
        EmitVertex();
        //next position
        gl_Position = vec4(-0.5 + 0.2 * cos(angle + segment), -0.5 + 0.2 * sin(angle + segment)* aspect, 0.0, 1.0);
        EmitVertex();
        //center
        gl_Position = vec4(-0.5, -0.5, 0.0, 1.0);
        EmitVertex();

        EndPrimitive();
    }
}

void maked(){
    //obj file content
    //in reality, we should load the obj file from disk into VBO
    vec3 v[18] = vec3[18](
        vec3( -0.112173, 0.000000, -0.353303), //1
        vec3( 0.144138, 0.000000, -0.353303),  //2
        vec3( 0.144138, 0.000000, 0.100063),   //3
        vec3( 0.182006, 0.000000, 0.100063),   //4
        vec3( 0.182006, 0.000000, 0.280638),   //5
        vec3( 0.132217, 0.000000, 0.280638),   //6
        vec3( 0.132217, 0.000000, 0.160722),   //7
        vec3( -0.146184, 0.000000, 0.160722),  //8
        vec3( -0.146184, 0.000000, 0.280638),  //9
        vec3( -0.195974, 0.000000, 0.280638),  //10
        vec3( -0.195974, 0.000000, 0.100063),  //11
        vec3( -0.163365, 0.000000, 0.100063),  //12
        vec3( 0.088388, 0.000000, -0.292644),  //13
        vec3( -0.058527, 0.000000, -0.292644), //14
        vec3( -0.105862, 0.000000, 0.100063),  //15
        vec3( 0.088388, 0.000000, 0.100063),   //16
        vec3( -0.112173, 0.000000, -0.353303), //17
        vec3( -0.058527, 0.000000, -0.292644)  //18
    );
    int faces[48]= int[48](
        14, 13, 2,
        13, 3, 2,
        16, 3, 13,
        14, 2, 1, 
        10, 12, 11,
        10, 8, 12,
        8, 15, 12,
        8, 16, 15,
        8, 3, 16,
        8, 4, 3,
        8, 7, 4,
        7, 5, 4,
        10, 9, 8,
        6, 5, 7,
        14, 1, 12,
        15, 14, 12
    );
    for(int i=0;i<16;i++){
        fColor = vec3(0.5, 0.0, 0.0);
        for(int j=0;j<3;j++){
            gl_Position = vec4(v[faces[i*3+j]-1].x*1+0.4, -v[faces[i*3+j]-1].z*1-0.4, 0.0, 1.0);
            EmitVertex();
        }
        EndPrimitive();
    }
}
/*
ЗАВДАННЯ
1. Створити чотири точки на екрані, з’єднати їх лініями.  (makesquare)
2. Створити трикутник (maketriangle)
3. З’єднати точки лініями. Я використовую GL_POINTS бо це геометричний шейдер
4. Я можу робити тільки стріпи, тому що це геометричний шейдер
5. Створити коло: (makecircle)
- створення кола відбувається з використанням стріпа
6. За допомогою ліній намалюйте першу літеру вашого прізвища і ім&#39;я. (maked)
*/
void main() {    
    makesquare();
    maketriangle();
    makecircle();
    maked();
}
