#include "loaders.hpp"
using namespace std;

struct Face
{
    int pos;
    int uv;
    int n;
    Face(std::vector<string> s)
    {
        pos = atoi(s[0].c_str());
        uv = atoi(s[1].c_str());
        n = atoi(s[2].c_str());
    }
};

bool loadOBJ(const char *filename, vector<glm::vec3> &vertices, vector<glm::vec2> &uvs, vector<glm::vec3> &normals)
{
    ifstream obj(filename);
    if (!obj.is_open())
    {
        cout << "Error: can't open file " << filename << endl;
        return false;
    }
    std::vector<glm::vec3> vertice;
    std::vector<glm::vec2> uv;
    std::vector<glm::vec3> normal;

    for (string buffer; getline(obj, buffer);)
    {
        if (buffer[0] == 'v' && buffer[1] == ' ')
        {
            buffer.erase(0, 2);
            vertice.push_back(string2vec3(split(buffer)));
            continue;
        }
        if (buffer[0] == 'v' && buffer[1] == 't')
        {
            buffer.erase(0, 3);
            uv.push_back(string2vec2(split(buffer)));
            continue;
        }
        if (buffer[0] == 'v' && buffer[1] == 'n')
        {
            buffer.erase(0, 3);
            normal.push_back(string2vec3(split(buffer)));
            continue;
        }
        if (buffer[0] == 'f' && buffer[1] == ' ')
        {
            // f 2/1/1 3/2/1 4/3/1
            // remove f letter
            buffer.erase(0, 2);

            vector<Face> faces;
            for (string fstr : split(buffer, " "))
            {
                std::vector<string> vstrs = split(fstr, "/");
                Face f(vstrs);
                faces.push_back(f);
            }
            for (int i = 0; i < faces.size(); i++)
            {
                vertices.push_back(vertice[faces[i].pos - 1]);
                uvs.push_back(uv[faces[i].uv - 1]);
                normals.push_back(normal[faces[i].n - 1]);
            }
            continue;
        } // parser works.
    }
    obj.close();
    return true;
}