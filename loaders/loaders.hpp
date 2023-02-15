#include "../libs.hpp"
#include "../utils/vectorfp.hpp"
#include <vector>
#include <fstream>

#ifndef LOADERS_HPP
#define LOADERS_HPP

using namespace std;

// loads an .obj file into a vector of vertices, uvs, and normals
bool loadOBJ(const char *filename, vector<glm::vec3> &vertices, vector<glm::vec2> &uvs, vector<glm::vec3> &normals);

typedef struct
{
    unsigned char *data;
    int width;
    int height;
} texturedata;

texturedata loadpng_raw(const char *filename);

// loads a .png file into a texture
GLuint loadpng(const char *filename);

// loads .png files into a cubemap
GLuint loadpngSkybox(vector<string> faces);

// loads shaders from files into a program
GLuint LoadShaders(const char *vertex_file_path, const char *fragment_file_path, const char *geometry_file_path = NULL);

// loads a .ttf file into a font
void loadfont(const char *filename);

// visualizes a string in a font
void drawstring(char font, string text, float x, float y, float sx, float sy);

#endif