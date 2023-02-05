#include "vectorfp.hpp"
using namespace std;

vector<string> split(string s, string delimiter)
{
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != string::npos)
    {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}
vector<int> intize(vector<string> s)
{
    vector<int> res;
    for (string str : s)
        res.push_back(atoi(str.c_str()));
    return res;
}
glm::vec3 string2vec3(vector<string> s)
{
    return glm::vec3(
        stod(s[0].c_str(), 0x0),
        stod(s[1].c_str(), 0x0),
        stod(s[2].c_str(), 0x0));
}
glm::vec2 string2vec2(vector<string> s)
{
    return glm::vec2(
        stod(s[0].c_str(), 0x0),
        -stod(s[1].c_str(), 0x0)); // invert y
}