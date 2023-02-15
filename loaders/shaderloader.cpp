#include "loaders.hpp"

// forward declaration
GLuint compileShader(const char *path, GLenum shaderType);

GLuint LoadShaders(const char *vertex_file_path, const char *fragment_file_path, const char *geometry_file_path)
{

    GLuint VertexShaderID = compileShader(vertex_file_path, GL_VERTEX_SHADER);
    GLuint FragmentShaderID = compileShader(fragment_file_path, GL_FRAGMENT_SHADER);
    GLuint GeometryShaderID;
    if (geometry_file_path != NULL)
        GeometryShaderID = compileShader(geometry_file_path, GL_GEOMETRY_SHADER);
    // Link the program
    printf("Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    if (GeometryShaderID != NULL)
        glAttachShader(ProgramID, GeometryShaderID);

    glLinkProgram(ProgramID);

    // Check the program
    GLint Result = GL_FALSE;
    int InfoLogLength;
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
    if (GeometryShaderID != NULL)
        glDetachShader(ProgramID, GeometryShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    if (geometry_file_path != NULL)
        glDeleteShader(GeometryShaderID);

    return ProgramID;
}

string getShaderCode(const char *path)
{
    // Read the Vertex Shader code from the file
    std::string ShaderCode;
    std::ifstream ShaderStream(path, std::ios::in);
    if (ShaderStream.is_open())
    {
        std::stringstream sstr;
        sstr << ShaderStream.rdbuf();
        ShaderCode = sstr.str();
        ShaderStream.close();
    }
    else
    {
        printf("Impossible to open %s. Are you in the right directory ?\n", path);
        exit(0);
    }
    return ShaderCode;
}

GLuint compileShader(const char *path, GLenum shaderType)
{
    GLuint shaderID = glCreateShader(shaderType);
    std::string shaderCode = getShaderCode(path);
    printf("Compiling shader : %s \n", path);
    char const *shaderSourcePointer = shaderCode.c_str();
    glShaderSource(shaderID, 1, &shaderSourcePointer, NULL);
    glCompileShader(shaderID);

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Check Vertex Shader
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0)
    {
        std::vector<char> shaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(shaderID, InfoLogLength, NULL, &shaderErrorMessage[0]);
        printf("%s\n", &shaderErrorMessage[0]);
    }
    return shaderID;
}