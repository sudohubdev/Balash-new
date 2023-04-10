#include "NURBS.hpp"

float knotSpacing(int degree, float knot)
{
    return (degree > 0) ? knotSpacing(degree - 1, knot) + knotSpacing(degree - 1, knot + 1) : 1.0f;
}

float basisFunction(float t, int degree, float knot)
{
    if (degree == 0) {
        // Base case: a piecewise constant function
        if (knot <= t && t < knot + 1) {
            return 1.0f;
        } else {
            return 0.0f;
        }
    } else {
        // Recursive case: a weighted sum of lower-degree basis functions
        float weight1 = 0.0f, weight2 = 0.0f;
        float basis1 = 0.0f, basis2 = 0.0f;

        weight1 = (t - knot) / (degree * knotSpacing(degree, knot));
        weight2 = (knot + degree + 1 - t) / (degree * knotSpacing(degree, knot + 1));

        basis1 = basisFunction(t, degree - 1, knot);
        basis2 = basisFunction(t, degree - 1, knot + 1);

        return weight1 * basis1 + weight2 * basis2;
    }
}

NURBS::NURBS(std::vector<glm::vec4> points, int degreeU, int degreeV, int resolutionU, int resolutionV): Geometry()
{
    // generate vertices of triangle mesh
    this->vertices = std::vector<glm::vec3>();
    this->vertices.reserve(resolutionU * resolutionV);
    for (int i = 0; i < resolutionU; i++)
    {
        for (int j = 0; j < resolutionV; j++)
        {
            float u = static_cast<float>(i) / (resolutionU - 1);
            float v = static_cast<float>(j) / (resolutionV - 1);
            glm::vec3 vertex(0.0f);
            float weightSum = 0.0f;
            for (int k = 0; k < points.size(); k++)
            {
                float basisU = basisFunction(u, degreeU, k);
                float basisV = basisFunction(v, degreeV, k);
                float weight = basisU * basisV * points[k].w;
                vertex += glm::vec3(points[k].x, points[k].y, points[k].z) * weight;
                weightSum += weight;
            }
            vertex /= weightSum;
            this->vertices.push_back(vertex);
        }
    }
    // generate uvs and normals
    this->uvs = std::vector<glm::vec2>();
    this->uvs.reserve(resolutionU * resolutionV);
    this->normals = std::vector<glm::vec3>();
    this->normals.reserve(resolutionU * resolutionV);
    for (int i = 0; i < resolutionU; i++)
    {
        for (int j = 0; j < resolutionV; j++)
        {
            this->uvs.push_back(glm::vec2(i / (float)resolutionU, j / (float)resolutionV));
            this->normals.push_back(glm::vec3(0, 0, 0));
        }
    }
}

NURBS::~NURBS()
{
    //
}
