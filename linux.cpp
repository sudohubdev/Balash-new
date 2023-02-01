#include <GL/glew.h>
#include <GLFW/glfw3.h>
#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <libpng16/png.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <png.h>
#endif
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstring>
#include <assert.h>
using namespace std;
char *strsep(char **stringp, const char *delim)
{
    char *rv = *stringp;
    if (rv)
    {
        *stringp += strcspn(*stringp, delim);
        if (**stringp)
            *(*stringp)++ = '\0';
        else
            *stringp = 0;
    }
    return rv;
}
ssize_t getline(char **lineptr, size_t *n, FILE *stream)
{
    size_t pos;
    int c;

    if (lineptr == NULL || stream == NULL || n == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    c = getc(stream);
    if (c == EOF)
    {
        return -1;
    }

    if (*lineptr == NULL)
    {
        *lineptr = (char *)malloc(128);
        if (*lineptr == NULL)
        {
            return -1;
        }
        *n = 128;
    }

    pos = 0;
    while (c != EOF)
    {
        if (pos + 1 >= *n)
        {
            size_t new_size = *n + (*n >> 2);
            if (new_size < 128)
            {
                new_size = 128;
            }
            char *new_ptr = (char *)realloc(*lineptr, new_size);
            if (new_ptr == NULL)
            {
                return -1;
            }
            *n = new_size;
            *lineptr = new_ptr;
        }

        ((unsigned char *)(*lineptr))[pos++] = c;
        if (c == '\n')
        {
            break;
        }
        c = getc(stream);
    }

    (*lineptr)[pos] = '\0';
    return pos;
}
GLuint platspec_loadpng(const char *filename)
{
    unsigned int width, height;
    png_byte color_type;
    png_byte bit_depth;
    png_structp png_ptr;
    png_infop info_ptr;
    png_bytep *row_pointers;

    char header[8]; // 8 is the maximum size that can be checked

    /* open file and test for it being a png */
    FILE *fp = fopen(filename, "rb");
    if (!fp)
        perror("[read_png_file] File %s could not be opened for reading");
    fread(header, 1, 8, fp);
    if (png_sig_cmp((png_const_bytep)header, 0, 8))
        perror("[read_png_file] File %s is not recognized as a PNG file");
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
        perror("[read_png_file] png_create_read_struct failed");
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
        perror("[read_png_file] png_create_info_struct failed");
    if (setjmp(png_jmpbuf(png_ptr)))
        perror("[read_png_file] Error during init_io");

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
    // reading shit:
    row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * height);
    size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);

    unsigned char *buffer = (unsigned char *)::malloc(rowbytes * height);

    for (unsigned int y = 0; y < height; y++)
        row_pointers[y] = (png_bytep)malloc(rowbytes);
    png_read_image(png_ptr, (png_bytep *)row_pointers);
    fclose(fp);
    for (unsigned int y = 0; y < height; y++)
        memcpy(buffer + (rowbytes * y), row_pointers[y], rowbytes);

    // creating texture:
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    // freeing up memory:
    free(buffer);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    for (unsigned int y = 0; y < height; y++)
        free(row_pointers[y]);
    free(row_pointers);
    png_free(png_ptr, NULL);
    return textureID;
}

bool platspec_loadOBJ(const char *filename, vector<glm::vec3> &vertices, vector<glm::vec2> &uvs, vector<glm::vec3> &normals)
{
    FILE *obj = fopen(filename, "r");
    char *buffer; // buffer to read lines of file strs
    buffer = (char *)malloc(10000);
    char *fstr; // Face STRing
    char *vstr; // Value STRing
    std::vector<glm::vec3> vertice;
    std::vector<glm::vec2> uv;
    std::vector<glm::vec3> normal;
    size_t len = 0; // file shit
    ssize_t read;   // file shit

    while ((read = getline(&buffer, &len, obj)) != -1)
    {
        if (buffer[0] == 'v' && buffer[1] == ' ')
        {
            char *shit[3];
            strsep(&buffer, " ");
            shit[0] = strsep(&buffer, " ");
            shit[1] = strsep(&buffer, " ");
            shit[2] = strsep(&buffer, " ");

            vertice.push_back(glm::vec3(
                strtof(shit[0], 0x0), // try to change to float later
                strtof(shit[1], 0x0),
                strtof(shit[2], 0x0))); // w is just 1
            continue;
        }
        if (buffer[0] == 'v' && buffer[1] == 't')
        {
            char *shit[2];
            strsep(&buffer, " ");
            shit[0] = strsep(&buffer, " ");
            shit[1] = strsep(&buffer, " ");
            uv.push_back(glm::vec2(
                strtof(shit[0], 0x0),
                -strtof(shit[1], 0x0)));
            continue;
        }
        if (buffer[0] == 'v' && buffer[1] == 'n')
        {
            char *shit[3];
            strsep(&buffer, " ");
            shit[0] = strsep(&buffer, " ");
            shit[1] = strsep(&buffer, " ");
            shit[2] = strsep(&buffer, " ");
            normal.push_back(glm::vec3(
                strtof(shit[0], 0x0),
                strtof(shit[1], 0x0),
                strtof(shit[2], 0x0)));
            continue;
        }
        if (buffer[0] == 'f' && buffer[1] == ' ')
        {
            // f 2/1/1 3/2/1 4/3/1
            strsep(&buffer, " "); // remove f letter
            int shit[3];
            int shit2[3];
            int shit3[3];
            int m = 0;
            while ((fstr = strsep(&buffer, " ")) != 0)
            {                              // iter faces thru spaces. got 2/1/1 in fstr
                vstr = strsep(&fstr, "/"); // got 2 from fstr
                shit[m] = atoi(vstr);
                vstr = strsep(&fstr, "/"); // got 1 from fstr
                shit2[m] = atoi(vstr);
                vstr = strsep(&fstr, "/"); // got 1 from fstr
                shit3[m] = atoi(vstr);
                m++;
            }
            vertices.push_back(vertice[shit[0] - 1]);
            vertices.push_back(vertice[shit[1] - 1]);
            vertices.push_back(vertice[shit[2] - 1]);
            uvs.push_back(uv[shit2[0] - 1]);
            uvs.push_back(uv[shit2[1] - 1]);
            uvs.push_back(uv[shit2[2] - 1]);
            normals.push_back(normal[shit3[0] - 1]);
            normals.push_back(normal[shit3[1] - 1]);
            normals.push_back(normal[shit3[2] - 1]);

        } // parser works.
    }
    fclose(obj);
    return true;
}

using namespace glm;

GLFWwindow *win;

int resx = 640, resy = 480;

GLuint VertexArrayID;
GLuint vertexbuffer;
GLuint uvbuffer;
// tsk i need to take a baf brb
// but where to take that shaders? GLSL?
GLuint LoadShaders(const char *vertex_file_path, const char *fragment_file_path)
{

    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER); // ok learning code rn
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if (VertexShaderStream.is_open())
    {
        std::stringstream sstr;
        sstr << VertexShaderStream.rdbuf();
        VertexShaderCode = sstr.str();
        VertexShaderStream.close();
    }
    else
    {
        printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
        exit(0);
    }

    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if (FragmentShaderStream.is_open())
    {
        std::stringstream sstr;
        sstr << FragmentShaderStream.rdbuf();
        FragmentShaderCode = sstr.str();
        FragmentShaderStream.close();
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
    printf("Compiling shader : %s\n", vertex_file_path);
    char const *VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0)
    {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }

    // Compile Fragment Shader
    printf("Compiling shader : %s\n", fragment_file_path);
    char const *FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0)
    {
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
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
    if (InfoLogLength > 0)
    {
        std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }

    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}
GLuint textureIDblyat;
GLuint programID;

static std::vector<glm::vec3> vertices;
void platspec_init()
{
    glewExperimental = true;

    if (!glfwInit())
    {
        exit(-1);
    }
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL

    win = glfwCreateWindow(resx, resy, "balash", NULL, NULL);
    if (!win)
        exit(-2);

    glfwMakeContextCurrent(win);
    glewExperimental = true; // Needed in core profile

    GLenum glewErr = glewInit();
    if (glewErr != GLEW_OK && glewErr != GLEW_ERROR_NO_GLX_DISPLAY)
        exit(-3);

    glfwSetInputMode(win, GLFW_STICKY_KEYS, GL_TRUE);

    // making buffers
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    glActiveTexture(GL_TEXTURE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);

    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;

    bool res = platspec_loadOBJ("murlok.obj", vertices, uvs, normals);

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    // UV THERE
    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), uvs.data(), GL_STATIC_DRAW);

    // Create one OpenGL texture
    textureIDblyat = platspec_loadpng("murlok.png");
    programID = LoadShaders("./shad.vsh", "./shad.fsh");
}

void platspec_glclear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 1st attribute buffer : vertices
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glUseProgram(programID);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
        0,        // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,        // size
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,        // stride
        (void *)0 // array buffer offset
    );

    // 2nd attribute buffer : uv
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(
        1,        // attribute. No particular reason for 1, but must match the layout in the shader.
        2,        // size TODO:
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,        // stride
        (void *)0 // array buffer offset
    );

    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
    glm::mat4 View = glm::lookAt(
        glm::vec3(0, 0, 2), // Camera is at (0,3,3), in World Space
        glm::vec3(0, 0, 0), // and looks at the origin
        glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );
    glm::mat4 Model = glm::mat4(1.0f);
    glm::mat4 mvp = Projection * View * Model; // Remember, matrix multiplication is the other way around
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

    // Draw the triangle !
    glActiveTexture(GL_TEXTURE0);                   // Hey OpenGL, we're about to give commands for texture sampler 0.
    glBindTexture(GL_TEXTURE_2D, textureIDblyat);   // when the currently-active sampler samples from a 2D texture, this is the texture to use.
    glEnable(GL_TEXTURE_2D);                        // the currently-active sampler should actually use the bound 2D texture.
    glDrawArrays(GL_TRIANGLES, 0, vertices.size()); // Starting from vertex 0; 3 vertices total -> 1 triangle

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void platspec_term()
{
    glfwDestroyWindow(win);
    glfwTerminate();
}

int platspec_termprog()
{
    glfwSwapBuffers(win);
    glfwPollEvents();
    return glfwWindowShouldClose(win);
}
