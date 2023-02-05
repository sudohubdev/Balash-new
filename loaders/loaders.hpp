#include "../libs.hpp"
#include "../utils/vectorfp.hpp"
#include <vector>
#include <fstream>

using namespace std;

// loads an .obj file into a vector of vertices, uvs, and normals
bool loadOBJ(const char *filename, vector<glm::vec3> &vertices, vector<glm::vec2> &uvs, vector<glm::vec3> &normals);

// loads a .png file into a texture
GLuint platspec_loadpng(const char *filename);

// loads shaders from files into a program
GLuint LoadShaders(const char *vertex_file_path, const char *fragment_file_path);