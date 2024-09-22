#include "NURBS.hpp"

bool is_inside_triangle(glm::vec3 u, glm::vec3 v, glm::vec3 prev, glm::vec3 next) {
    // Compute the barycentric coordinates of the point with respect to the triangle
    glm::vec3 e1 = prev - v;
    glm::vec3 e2 = next - v;
    glm::vec3 e3 = u - v;
    float det = glm::determinant(glm::mat3(e1, e2, glm::vec3(0, 0, 1)));
    if (det == 0) {
        return false;
    }
    float lambda1 = glm::determinant(glm::mat3(e3, e2, glm::vec3(0, 0, 1))) / det;
    float lambda2 = glm::determinant(glm::mat3(e1, e3, glm::vec3(0, 0, 1))) / det;
    float lambda3 = 1 - lambda1 - lambda2;

    // Check if the barycentric coordinates are inside the triangle
    return lambda1 >= 0 && lambda2 >= 0 && lambda3 >= 0;
}

bool is_convex(glm::vec3 v, glm::vec3 prev, glm::vec3 next) {
    glm::vec2 u = glm::vec2(prev.x - v.x, prev.y - v.y);
    glm::vec2 w = glm::vec2(next.x - v.x, next.y - v.y);
    float cross_product = u.x * w.y - u.y * w.x;
    return cross_product > 0;
}


bool is_ear(glm::vec3 v, glm::vec3 prev, glm::vec3 next, const std::vector<glm::vec3>& vertices) {
    // Check if v is a convex vertex
    if (!is_convex(v, prev, next)) {
        return false;
    }

    // Check if no other vertex is inside the triangle (v, prev, next)
    for (glm::vec3 u : vertices) {
        if (u == v || u == prev || u == next) {
            continue;
        }
        if (is_inside_triangle(u, v, prev, next)) {
            return false;
        }
    }

    return true;
}




std::vector<glm::vec3> vertices2surface(std::vector<glm::vec3> vertices) {
    // Create a list of edges connecting the vertices
    std::vector<std::pair<glm::vec3, glm::vec3>> edges;
    for (int i = 0; i < vertices.size(); i++) {
        glm::vec3 v1 = vertices[i];
        glm::vec3 v2 = vertices[(i + 1) % vertices.size()];
        edges.push_back(std::make_pair(v1, v2));
    }

    // Create a list of triangles
    std::vector<glm::vec3> triangles;

    // Iterate until there are no more ears in the polygon
    while (vertices.size() > 3) {
        // Find an ear of the polygon
        int ear_index = -1;
        for (int i = 0; i < vertices.size(); i++) {
            glm::vec3 v = vertices[i];
            glm::vec3 prev = vertices[(i + vertices.size() - 1) % vertices.size()];
            glm::vec3 next = vertices[(i + 1) % vertices.size()];
            if (is_ear(v, prev, next, vertices)) {
                ear_index = i;
                break;
            }
        }
        if (ear_index == -1) {
            break;
        }

        // Remove the ear from the polygon
        glm::vec3 v = vertices[ear_index];
        glm::vec3 prev = vertices[(ear_index + vertices.size() - 1) % vertices.size()];
        glm::vec3 next = vertices[(ear_index + 1) % vertices.size()];
        vertices.erase(vertices.begin() + ear_index);
        edges.erase(edges.begin() + ear_index);
        if (ear_index == vertices.size()) {
            ear_index = 0;
        }
        edges[ear_index].first = prev;
        edges[(ear_index + 1) % edges.size()].second = next;

        // Add the ear to the mesh
        triangles.push_back(v);
        triangles.push_back(prev);
        triangles.push_back(next);
    }

    // Add the last triangle
    triangles.push_back(vertices[0]);
    triangles.push_back(vertices[1]);
    triangles.push_back(vertices[2]);

    return triangles;
}




//інтерполяційна функція Б сплайну

float B0(float u){
    return float(pow(1-u, 3) / 6.0);
}

float B1(float u){
    return float((3 * pow(u, 3) - 6 * pow(u, 2) + 4) / 6.0);
}

float B2(float u){
    return float((-3 * pow(u, 3) + 3 * pow(u, 2) + 3 * u + 1) / 6.0);
}

float B3(float u){
    return float(pow(u, 3) / 6.0);
}

glm::vec3 interpolate(float u, int i, std::vector<glm::vec4> points){
    int aa = (i + 1) % points.size();
    int bb = (i + 2) % points.size();
    int cc = (i + 3) % points.size();

    glm::vec4 Q = B0(u) * points[i] + B1(u) * points[aa] + B2(u) * points[bb] + B3(u) * points[cc];
    //Q /= Q.w;
    return glm::vec3(Q.x, Q.y, Q.z);
}


NURBS::NURBS(std::vector<glm::vec4> points, int resolution): Geometry()
{
    // generate vertices of triangle mesh
    this->vertices = std::vector<glm::vec3>();
    this->uvs = std::vector<glm::vec2>();
    this->uvs.reserve(resolution * resolution);
    this->normals = std::vector<glm::vec3>();
    this->normals.reserve(resolution * resolution);
    
    if(resolution==0){
        for (int i = 0; i < points.size(); i++)
        {
            this->vertices.push_back(glm::vec3(points[i].x, points[i].y, points[i].z));
            //calculate normals
            glm::vec3 norm = glm::vec3(0, 0, 0);
            if (i > 0)
            {
                norm += glm::cross(glm::vec3(points[i].x - points[i - 1].x, points[i].y - points[i - 1].y, points[i].z - points[i - 1].z), glm::vec3(0, 0, 1));
            }
            if (i < points.size() - 1)
            {
                norm += glm::cross(glm::vec3(points[i + 1].x - points[i].x, points[i + 1].y - points[i].y, points[i + 1].z - points[i].z), glm::vec3(0, 0, 1));
            }
            this->normals.push_back(glm::normalize(norm));
        }
        goto aaa;
    }
    for(int p=0; p<points.size(); p++)
    {
        for(int i=0; i<resolution; i++)
        {
            float u = float(i) / float(resolution);
            glm::vec3 vertex = interpolate(u, p, points);
            this->vertices.push_back(vertex);
            //нормалі
            glm::vec3 norm = interpolate(u + 0.01, p, points);
            glm::vec3 d = norm - vertex;
            float len = sqrt(d.x*d.x + d.y*d.y + d.z*d.z);
            glm::vec3 normal = d / len;
            this->normals.push_back(normal);
        }
    }
    aaa:
    this->vertices = vertices2surface(this->vertices);


    //bounding box
    glm::vec3 maxcoords = glm::vec3(0, 0, 0);
    glm::vec3 mincoords = glm::vec3(0, 0, 0);
    //get the max coords of the vertices
    for (int i = 0; i < this->vertices.size(); i++)
    {
        maxcoords.x = std::max(maxcoords.x, this->vertices[i].x);
        maxcoords.y = std::max(maxcoords.y, this->vertices[i].y);
        maxcoords.z = std::max(maxcoords.z, this->vertices[i].z);
        mincoords.x = std::min(mincoords.x, this->vertices[i].x);
        mincoords.y = std::min(mincoords.y, this->vertices[i].y);
        mincoords.z = std::min(mincoords.z, this->vertices[i].z);
    }
    glm::vec3 size = maxcoords - mincoords;
    for (int i = 0; i < this->vertices.size(); i++)
    {
        this->uvs.push_back(glm::vec2((this->vertices[i].x - mincoords.x) / size.x, 1-(this->vertices[i].y - mincoords.y) / size.y));
        //shift the x and y coords to the center of the bounding box
        this->vertices[i].x -= (maxcoords.x + mincoords.x) / 2;
        this->vertices[i].y -= (maxcoords.y + mincoords.y) / 2;
    }
}

NURBS::~NURBS()
{
    //
}
