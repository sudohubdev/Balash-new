#include "../../libs.hpp"
#include "../../engine.hpp"
#include "Bone.hpp"
#include <map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#ifndef ANIMMESH_HPP
#define ANIMMESH_HPP

#define MAX_BONE_INFLUENCE 4
#define MAX_BONE_WEIGHTS 4
struct Vertex {
    // position
    glm::vec3 Position;
    //bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
};
struct BoneInfo
{
    /*id is index in finalBoneMatrices*/
    int id;
    /*offset matrix transforms vertex from model space to bone space*/
    glm::mat4 offset;
};


class AnimMesh : public Mesh
{
public:
    AnimMesh();
    AnimMesh(string path);
    ~AnimMesh();
    void genBuffers();
    void bindBuffers();
    void unbindBuffers();
    GLuint getVAO();
    void attachShader(GLuint shaderID);
    GLuint getShader();
    void setMVP(GLuint shaderID, glm::mat4 MVP);
    void setMVP(Camera *camera);
    GLsizei getVertexCount();
    AnimMesh *getParent();
    AnimMesh *setParent(AnimMesh *parent);
    std::vector<AnimMesh*> getChildren();
    AnimMesh *addChild(AnimMesh *child);
    void draw(Camera *camera);
    
    std::map<string, BoneInfo>  GetBoneInfoMap();
    int& GetBoneCount();

    void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);

    std::vector<Bone> mBones;

    std::vector<std::array<int, 4>> boneIDs;
    std::vector<glm::vec4> weights;
private:
    std::vector<AnimMesh*> children;
    AnimMesh *parent;

    std::map<string, BoneInfo> m_BoneInfoMap;
    int m_BoneCounter = 0;
    void SetVertexBoneDataToDefault(Vertex& vertex);
    AnimMesh processMesh(aiMesh* mesh, const aiScene* scene);
    void SetVertexBoneData(Vertex& vertex, int boneID, float weight);

    GLuint bonebuffer;
    GLuint weightbuffer;
};
#endif

/*
class Mesh
{
public:
    Mesh(Texture *texture, Geometry *geometry);
    Mesh();
    ~Mesh();
    void setTexture(Texture *texture);
    void setGeometry(Geometry *geometry);
    Texture *getTexture();
    Geometry *getGeometry();
    virtual void genBuffers();
    virtual void bindBuffers();
    GLuint getVAO();
    virtual void unbindBuffers();
    // may be used for particular mesh rendering
    virtual void attachShader(GLuint shaderID);
    virtual GLuint getShader();

    virtual void setMVP(GLuint shaderID, glm::mat4 MVP);
    virtual void setMVP(Camera *camera);
    virtual GLsizei getVertexCount();
    // basic mesh properties
    glm::vec3 position{0, 0, 0};
    glm::vec3 rotation{0, 0, 0};
    glm::vec3 scale{1, 1, 1};
    // moves mesh relative to its current position.
    // unlike position+=direction, this will move the mesh in the direction it is looking at
    void moveRelative(glm::vec3 move);
    glm::mat4 getModelMatrix();

protected:
    Texture *texture;
    Geometry *geometry;
    GLuint vertexbuffer;
    GLuint uvbuffer;
    GLuint normalbuffer;
    GLuint vao;
    GLuint shader;
};
*/