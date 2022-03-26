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

GLuint platspec_loadpng(const char * filename){
    unsigned int width,height;
    png_byte color_type;
    png_byte bit_depth;
    png_structp png_ptr;
    png_infop info_ptr;
    png_bytep*row_pointers;

    char header[8]; // 8 is the maximum size that can be checked

    /* open file and test for it being a png */
    FILE *fp = fopen(filename, "rb");
    if (!fp)perror("[read_png_file] File %s could not be opened for reading");
    fread(header, 1, 8, fp);
    if (png_sig_cmp((png_const_bytep)header, 0, 8))perror("[read_png_file] File %s is not recognized as a PNG file");
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)perror("[read_png_file] png_create_read_struct failed");
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)perror("[read_png_file] png_create_info_struct failed");
    if (setjmp(png_jmpbuf(png_ptr)))perror("[read_png_file] Error during init_io");

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    width = png_get_image_width(png_ptr, info_ptr);
    height = png_get_image_height(png_ptr, info_ptr);
    color_type = png_get_color_type(png_ptr, info_ptr);
    bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    /* Read any color_type into 8bit depth, RGBA format. */
    if (bit_depth == 16)
        png_set_strip_16(png_ptr);
    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);
    /* PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth. */
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png_ptr);
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png_ptr);
    /* These color_type don't have an alpha channel then fill it with 0xff. */
    if (color_type == PNG_COLOR_TYPE_RGB ||
        color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);
    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png_ptr);
    png_read_update_info(png_ptr, info_ptr);

    row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
    for (unsigned int y = 0; y < height; y++)
    {
        row_pointers[y] = (png_bytep )malloc(png_get_rowbytes(png_ptr, info_ptr));
    }
    png_read_image(png_ptr, (png_bytep*)row_pointers);
    fclose(fp);


    size_t imgbytes = width*height;
    glm::vec3 * buffer = new glm::vec3[imgbytes];//our buffer for img
    for (unsigned int y = 0; y <height; y++){
        for (unsigned int x = 0; x < width; x++){
            unsigned char *ptr = (unsigned char *)(&row_pointers[y][x]);
            size_t offset=x+y*width;
            buffer[offset]= glm::vec3(1,0,1);//glm::vec4((float)ptr[0]/255.0f,(float)ptr[1]/255.0f,(float)ptr[2]/255.0f,(float)ptr[3]/255.0f);
        }
    }
    // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    delete buffer;//delete buffer;
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    for (unsigned int y = 0; y < height; y++)
    {
        free(row_pointers[y]);
    }
    free(row_pointers);
    png_free(png_ptr,NULL);
    return textureID;
}



bool platspec_loadOBJ(const char * filename,vector<glm::vec3> &verticess,vector<glm::vec2> &uvs, vector<glm::vec3> &normals){
    FILE * obj = fopen(filename,"r");
    char * buffer;//buffer to read lines of file strs
    char * fstr;//Face STRing
    char * vstr;//Value STRing
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uv;
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
            uv.push_back(glm::vec2(
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
            int shit2 [3];
            int m =0;
            while((fstr = strsep(&buffer, " ")) != 0 ){//iter faces thru spaces. got 2/1/1 in fstr
                vstr = strsep(&fstr, "/");//got 2 from fstr
                shit[m]=atoi(vstr);
                vstr = strsep(&fstr, "/");//got 1 from fstr
                shit2[m]=atoi(vstr);
                m++;
            }
                verticess.push_back(vertices[shit[0]-1]);
                verticess.push_back(vertices[shit[1]-1]);
                verticess.push_back(vertices[shit[2]-1]);

                uvs.push_back(uv[shit2[0]-1]);
                uvs.push_back(uv[shit2[0]-1]);
                uvs.push_back(uv[shit2[0]-1]);
            
        }//parser works. 
    }
    fclose(obj); 
    return true;
}




using namespace glm;

GLFWwindow* win;

int resx=640,resy=480;

GLuint VertexArrayID;
GLuint vertexbuffer;
GLuint uvbuffer;
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
GLuint textureID;
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

    //making buffers
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
 
    glActiveTexture(GL_TEXTURE);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glDepthFunc(GL_LESS);
    // Accept fragment if it closer to the camera than the former one

	std::vector< glm::vec2 > uvs;
	std::vector< glm::vec3 > normals; // Won't be used at the moment.

    bool res = platspec_loadOBJ("cube.obj", vertices, uvs, normals);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    //UV THERE
    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, uvs.size()*sizeof(glm::vec2), uvs.data(), GL_STATIC_DRAW);



    // Create one OpenGL texture
    textureID=platspec_loadpng("test.png");
    glGenTextures(1, &textureID);
    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);


    programID=LoadShaders("./shad.vsh","./shad.fsh");
}

void platspec_glclear(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    // 1st attribute buffer : vertices
    glClearColor(1.0, 1.0, 1.0, 1.0);
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
    
    // 2nd attribute buffer : uv
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(
        1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
        2,                                // size TODO:
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
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
    glActiveTexture(GL_TEXTURE0);      // Hey OpenGL, we're about to give commands for texture sampler 0.
glBindTexture(GL_TEXTURE_2D, textureID); // when the currently-active sampler samples from a 2D texture, this is the texture to use.
glEnable(GL_TEXTURE_2D);           // the currently-active sampler should actually use the bound 2D texture.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
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
