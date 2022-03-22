#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstring>
#include <assert.h>
#include <png.h>
using namespace std;
bool platspec_loadOBJ(const char * filename,vector<glm::vec3> &verticess,vector<glm::vec2> &uvs, vector<glm::vec3> &normals, vector<GLushort> &elements,vector<GLushort> &uvelements){
    FILE * obj = fopen(filename,"r");
    char * buffer;//buffer to read lines of file strs
    char * fstr;//Face STRing
    char * vstr;//Value STRing
    std::vector<glm::vec3> vertices;

    size_t len = 0;//file shit
    ssize_t read;//file shit

    while ((read = getline(&buffer, &len, obj)) != -1) {
        if(buffer[0]=='v' && buffer[1]==' '){
            char* shit [3];
            strsep(&buffer, " ");
            shit[0]=strsep(&buffer, " ");
            shit[1]=strsep(&buffer, " ");
            shit[2]=strsep(&buffer, " ");
            
            vertices.push_back(glm::vec3(
            strtof(shit[0],0x0),   //try to change to float later
            strtof(shit[1],0x0),
            strtof(shit[2],0x0)
            ));//w is just 1
            continue;
        }
        if(buffer[0]=='v' && buffer[1]=='t'){
            char* shit [2];
            strsep(&buffer, " ");
            shit[0]=strsep(&buffer, " ");
            shit[1]=strsep(&buffer, " ");
            uvs.push_back(glm::vec2(
            strtof(shit[0],0x0),   
            strtof(shit[1],0x0)));
            continue;
        }
        if(buffer[0]=='v' && buffer[1]=='n'){
            char* shit [3];
            strsep(&buffer, " ");
            shit[0]=strsep(&buffer, " ");
            shit[1]=strsep(&buffer, " ");
            shit[2]=strsep(&buffer, " ");
            normals.push_back(glm::vec3(
            strtof(shit[0],0x0),   
            strtof(shit[1],0x0),
            strtof(shit[2],0x0)));
            continue;
        }
        if(buffer[0]=='f' && buffer[1]==' '){
            //f 2/1/1 3/2/1 4/3/1
            strsep(&buffer, " ");//remove f letter
            int shit [3];
            //int shit2 [3];
            int m =0;
            while((fstr = strsep(&buffer, " ")) != 0 ){//iter faces thru spaces. got 2/1/1 in fstr
                vstr = strsep(&fstr, "/");//got 2 from fstr
                shit[m]=atoi(vstr);
                //vstr = strsep(&fstr, "/");//got 1 from fstr
                //shit2[m]=atoi(vstr);
                m++;
            }
                verticess.push_back(vertices[shit[0]-1]);
                verticess.push_back(vertices[shit[1]-1]);
                verticess.push_back(vertices[shit[2]-1]);
            
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
    
    return true;
}




using namespace glm;

GLFWwindow* win;

int resx=640,resy=480;

GLuint VertexArrayID;
GLuint vertexbuffer;
//tsk i need to take a baf brb 
//but where to take that shaders? GLSL? 
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path){

	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);//ok learning code rn
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open()){
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}else{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		exit(0);
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}
	
	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);
	
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

GLuint programID;

	std::vector< glm::vec3> vertices;
void platspec_init(){
    glewExperimental=true;
    
    if(!glfwInit()){
        exit(-1);
    }
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

    
    win=glfwCreateWindow(resx,resy,"balash",NULL,NULL);
    if(!win){
        exit(-2);
    }
    
    glfwMakeContextCurrent(win);
    glewExperimental=true; // Needed in core profile
    
    //if(glewInit()!=GLEW_OK){
        //exit(-3);
    //}
  GLenum glewErr = glewInit();
  if (glewErr != GLEW_OK && glewErr != GLEW_ERROR_NO_GLX_DISPLAY) {
exit(-3);
}
    
    glfwSetInputMode(win, GLFW_STICKY_KEYS, GL_TRUE);
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    
    glGenBuffers(1, &vertexbuffer);
// The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
// Give our vertices to OpenGL.
//glEnable(GL_DEPTH_TEST);
// Accept fragment if it closer to the camera than the former one
//glDepthFunc(GL_LESS);


	std::vector< glm::vec2 > uvs;
	std::vector< glm::vec3 > normals; // Won't be used at the moment.
	std::vector< GLushort > elements; // Won't be used at the moment.
	std::vector< GLushort > uvelements; // Won't be used at the moment.

    bool res = platspec_loadOBJ("cube.obj", vertices, uvs, normals,elements,uvelements);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), &g_vertex_buffer_data, GL_STATIC_DRAW);
    //glBufferData sends data to shaders right? FINALLY
    programID=LoadShaders("./shad.vsh","./shad.fsh");
}

void platspec_glclear(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    // 1st attribute buffer : vertices
    
	glUseProgram(programID);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
   0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
   3,                  // size
   GL_FLOAT,           // type
   GL_FALSE,           // normalized?
   0,                  // stride
   (void*)0            // array buffer offset
    );
    
// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
  
// Or, for an ortho camera :
//glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates
  
// Camera matrix
glm::mat4 View = glm::lookAt(
    glm::vec3(10,10,10), // Camera is at (0,3,3), in World Space
    glm::vec3(0,0,0), // and looks at the origin
    glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
    );
  
// Model matrix : an identity matrix (model will be at the origin)
glm::mat4 Model = glm::mat4(1.0f);
// Our ModelViewProjection : multiplication of our 3 matrices
glm::mat4 mvp = Projection * View * Model; // Remember, matrix multiplication is the other way around
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
  
	// Send our transformation to the currently bound shader, in the "MVP" uniform
	// This is done in the main loop since each model will have a different MVP matrix (At least for the M part)
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);



    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() * 3); // Starting from vertex 0; 3 vertices total -> 1 triangle

    glDisableVertexAttribArray(0);

}

void platspec_term(){
    
    glfwDestroyWindow(win);
    glfwTerminate();
    
}

int platspec_termprog(){
    glfwSwapBuffers(win);
    glfwPollEvents();
    
    return glfwWindowShouldClose(win);
}
