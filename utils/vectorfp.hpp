
#include <vector>
#include <string>
#include <glm/glm.hpp>
using namespace std;

#ifndef VECTORFP_HPP
#define VECTORFP_HPP

vector<string> split(string s, string delimiter = " ");
vector<int> intize(vector<string> s);
glm::vec3 string2vec3(vector<string> s);
glm::vec2 string2vec2(vector<string> s);

#endif