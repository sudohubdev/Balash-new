
#include <glm/glm.hpp>
/*
#include <glm/glm.hpp>
// Read our .obj file
std::vector< glm::vec3 > vertices;
std::vector< glm::vec2 > uvs;
std::vector< glm::vec3 > normals; // Won't be used at the moment.
bool res = loadOBJ("cube.obj", vertices, uvs, normals);
*/
//don't forget to delete, it is malloced
bool platspec_loadOBJ(const char * filename,vector<glm::vec4> &vertices,vector<glm::vec4> &uvs, vector<glm::vec3> &normals, vector<GLushort> &elements,vector<GLushort> &uvelements){
    FILE * obj = fopen(filename,"r");
    char * buffer;//buffer to read lines of file
    char * fstr;//Face STRing
    char * vstr;//Value STRing


    size_t len = 0;//file shit
    ssize_t read;//file shit

    while ((read = getline(&buffer, &len, obj)) != -1) {
        if(buffer[0]=='v' && buffer[1]==' '){
            char* shit [3];
            strsep(&buffer, " ");
            shit[0]=strsep(&buffer, " ");
            shit[1]=strsep(&buffer, " ");
            shit[2]=strsep(&buffer, " ");
            
            vertices.push_back(glm::vec4(
            strtod(shit[0],0x0),   
            strtod(shit[1],0x0),
            strtod(shit[2],0x0),
            1);//w is just 1
            continue;
        }
        if(buffer[0]=='v' && buffer[1]=='t'){
            char* shit [2];
            strsep(&buffer, " ");
            shit[0]=strsep(&buffer, " ");
            shit[1]=strsep(&buffer, " ");
            uvs.push_back(glm::vec2(
            strtod(shit[0],0x0),   
            strtod(shit[1],0x0));
            continue;
        }
        if(buffer[0]=='v' && buffer[1]=='n'){
            char* shit [3];
            strsep(&buffer, " ");
            shit[0]=strsep(&buffer, " ");
            shit[1]=strsep(&buffer, " ");
            shit[2]=strsep(&buffer, " ");
            normalspush_back(glm::vec3(
            strtod(shit[0],0x0),   
            strtod(shit[1],0x0),
            strtod(shit[2],0x0));
            continue;
        }
        if(buffer[0]=='f' && buffer[1]==' '){
            //f 2/1/1 3/2/1 4/3/1
            strsep(&buffer, " ");//remove f letter
            int shit [3];
            int shit2 [3];
            while((fstr = strsep(&buffer, " ")) != 0 ){//iter faces thru spaces. got 2/1/1 in fstr
                vstr = strsep(&fstr, "/");//got 2 from fstr
                elements.push_back(atoi(vstr)-1); 
                vstr = strsep(&fstr, "/");//got 1 from fstr
                uvelements.push_back(atoi(vstr)-1); 
                //vstr = strsep(&fstr, "/");//TODO:normals?
                //nelements.push_back(atoi(vstr)-1); 
            }
        }//parser works. 
        
    }
    fclose(obj);
    
    /*
        normals.resize(vertices.size(), glm::vec3(0.0, 0.0, 0.0));
    for (int i = 0; i < elements.size(); i+=3)
    {
        GLushort ia = elements[i];
        GLushort ib = elements[i+1];
        GLushort ic = elements[i+2];
        glm::vec3 normal = glm::normalize(glm::cross(
        glm::vec3(vertices[ib]) - glm::vec3(vertices[ia]),
        glm::vec3(vertices[ic]) - glm::vec3(vertices[ia])));
        normals[ia] = normals[ib] = normals[ic] = normal;
    }
    //code that adds normals. need to port it
    */
    
    return ret;
}