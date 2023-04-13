#include "NURBS.hpp"




//triangulation

#include <glm/glm.hpp>
#include <vector>

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



const int splinepoints = 80;
const int flowerpoints = 27;
int totalpoints=0;
GLfloat points[][2] = {
    {0.3871, 0.1353},
    {0.3333, 0.1688},
    {0.4264, 0.3755},
    {0.4032, 0.4017},
    {0.2270, 0.2576},
    {0.1368, 0.3173},
    {0.1484, 0.4308},
    {0.4017, 0.5021},
    {0.4192, 0.5473},
    {0.1804, 0.6666},
    {0.1994, 0.7772},
    {0.2751, 0.8253},
    {0.4570, 0.5895},
    {0.5225, 0.5866},
    {0.6186, 0.8238},
    {0.7161, 0.8355},
    {0.7933, 0.7161},
    {0.5691, 0.5312},
    {0.5953, 0.4788},
    {0.8427, 0.4483},
    {0.8340, 0.3114},
    {0.7510, 0.2576},
    {0.5496, 0.4013},
    {0.5196, 0.3813},
    {0.5065, 0.1280},
    {0.3901, 0.1368},
    {0.3871, 0.1353},
    {0.3333, 0.1688},
    {0.4264, 0.3755},
    {0.4032, 0.4017}};


GLfloat pointsBSpline[3000][2];
GLfloat normals[3000][2];
//інтерполяційна функція Б сплайну (рекурсивна)
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

void computeBSpline(){
    int count = 0;
    for (int i = 0; i < flowerpoints; i++)
    {
        cout << "Computing for P" << i << " P " << i + 1 << " P " << i + 2 << " P " << i + 3 << endl;
        for (int j = 0; j < splinepoints; j++)
        {

            float u = float(j) / float(splinepoints);
            int aa = (i + 1) % flowerpoints;
            int bb = (i + 2) % flowerpoints;
            int cc = (i + 3) % flowerpoints;

            float Qx = B0(u) * points[i][0] + B1(u) * points[aa][0] + B2(u) * points[bb][0] + B3(u) * points[cc][0];
            float Qy = B0(u) * points[i][1] + B1(u) * points[aa][1] + B2(u) * points[bb][1] + B3(u) * points[cc][1];

            pointsBSpline[count][0] = Qx;
            pointsBSpline[count][1] = Qy;
            //calc normals
            float Qx1 = B0(u + 0.01) * points[i][0] + B1(u + 0.01) * points[aa][0] + B2(u + 0.01) * points[bb][0] + B3(u + 0.01) * points[cc][0];
            float Qy1 = B0(u + 0.01) * points[i][1] + B1(u + 0.01) * points[aa][1] + B2(u + 0.01) * points[bb][1] + B3(u + 0.01) * points[cc][1];
            
            float dx = Qx1 - Qx;
            float dy = Qy1 - Qy;
            float len = sqrt(dx*dx + dy*dy);
            normals[count][0] = dy / len;
            normals[count][1] = -dx / len;
            //calc tangents
            
            //cout << count << '(' << Qx << ", " << Qy << ")\n";
            count++;
        }
    }
    totalpoints = count;
    cout << "Computed " << count << " points!\n";
}


#include <vector>
#include <glm/glm.hpp>

// Calculate the point on the spline curve at parameter value u
glm::vec2 deboor(std::vector<glm::vec4> controlpoints, int splinepoints, float u) {
    int n = controlpoints.size() - 1;
    int k = std::min(n, splinepoints - 1);

    // Find the knot span index for the given parameter value
    int i;
    for (i = 0; i <= n; i++) {
        if (u < (i + 1) / float(n + k + 1)) {
            break;
        }
    }
    i = std::min(i, n - k);

    // Initialize the control point array
    std::vector<glm::vec2> d(k + 1);
    for (int j = 0; j <= k; j++) {
        d[j] = controlpoints[i - k + j];
    }

    // Evaluate the spline curve
    for (int r = 1; r <= k; r++) {
        for (int j = k; j >= r; j--) {
            float alpha = (u - (i - k + j) / float(n + k + 1)) / ((i + j - r + 1) / float(n + k + 1) - (i - k + j) / float(n + k + 1));
            d[j] = (1 - alpha) * d[j - 1] + alpha * d[j];
        }
    }

    return d[k];
}


NURBS::NURBS(std::vector<glm::vec4> points, int degreeU, int degreeV, int resolutionU, int resolutionV): Geometry()
{
    // generate vertices of triangle mesh
    this->vertices = std::vector<glm::vec3>();
    computeBSpline();
    //convert points to glm::vec4 vector
    std::vector<glm::vec4> points2;
    for(int i=0; i<totalpoints; i++)
    {
        points2.push_back(glm::vec4(pointsBSpline[i][0], pointsBSpline[i][1], 0, 1));
    }
    for(int i=0; i<totalpoints; i++)
    {
        this->vertices.push_back(glm::vec3(pointsBSpline[i][0], pointsBSpline[i][1], 0));
        //glm::vec2 cyka = deboor(points2, 10, i/float(totalpoints));
        //this->vertices.push_back(glm::vec3(cyka.x,cyka.y,0.0));
    }
    this->vertices = vertices2surface(this->vertices);
    // generate uvs and normals
    this->uvs = std::vector<glm::vec2>();
    this->uvs.reserve(resolutionU * resolutionV);
    this->normals = std::vector<glm::vec3>();
    this->normals.reserve(resolutionU * resolutionV);
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
        this->normals.push_back(glm::vec3(0, 0, 1));
    }
}

NURBS::~NURBS()
{
    //
}
