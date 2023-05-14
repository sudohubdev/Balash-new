#include "AnimMesh.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

GLuint ANIMSHADER = 0;

//process node
Geometry* processGeometry(aiMesh *mesh, const aiScene *scene);

void processNode(aiNode *node, const aiScene *scene, AnimMesh *parent)
{
    //process all the node's meshes (if any)
    for (GLuint i = 0; i < node->mNumMeshes; i++)
    {
        //the node object only contains indices to index the actual objects in the scene.
        //the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        //meshes.push_back(processMesh(mesh, scene));
        AnimMesh *animMesh = new AnimMesh();
        animMesh->setGeometry(processGeometry(mesh, scene));
        animMesh->genBuffers();
        animMesh->attachShader(ANIMSHADER);
        animMesh->setTexture(new Texture("assets/uvgrid.png"));
        if (parent != nullptr){
            animMesh->setParent(parent);
            parent->addChild(animMesh);
        }
    } 
    //then do the same for each of its children
    for (GLuint i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene, parent);
    }
}

Geometry* processGeometry(aiMesh *mesh, const aiScene *scene)
{
    Geometry* geometry = new Geometry();
    //process vertices
    for (GLuint i = 0; i < mesh->mNumVertices; i++)
    {
        glm::vec3 vertex;
        vertex.x = mesh->mVertices[i].x;
        vertex.y = mesh->mVertices[i].y;
        vertex.z = mesh->mVertices[i].z;
        geometry->vertices.push_back(vertex);
    }
    //process indices
    for (GLuint i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        //process each indices of the face
        for (GLuint j = 0; j < face.mNumIndices; j++)
        {
            geometry->indices.push_back(face.mIndices[j]);
        }
    }
    //process normals
    for (GLuint i = 0; i < mesh->mNumVertices; i++)
    {
        glm::vec3 normal;
        normal.x = mesh->mNormals[i].x;
        normal.y = mesh->mNormals[i].y;
        normal.z = mesh->mNormals[i].z;
        geometry->normals.push_back(normal);
    }
    //process uvs
    for (GLuint i = 0; i < mesh->mNumVertices; i++)
    {
        glm::vec2 uv;
        uv.x = mesh->mTextureCoords[0][i].x;
        uv.y = mesh->mTextureCoords[0][i].y;
        geometry->uvs.push_back(uv);
    }
    return geometry;
}


AnimMesh::AnimMesh(){
    this->children = std::vector<AnimMesh *>();
    this->parent = nullptr;
}
AnimMesh::AnimMesh(string path)
{
    this->children = std::vector<AnimMesh *>();
    this->parent = nullptr;

    Assimp::Importer importer;
    //we dont use indices
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_FlipUVs);
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
        return;
    }

    //loading shader
    if(ANIMSHADER == 0)
        ANIMSHADER = LoadShaders("shaders/anim/anim.vert", "shaders/anim/anim.frag");

    processNode(scene->mRootNode, scene, this);

    this->attachShader(ANIMSHADER);
    this->genBuffers();
}

AnimMesh::~AnimMesh()
{
    // deleting buffers
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &uvbuffer);
    glDeleteBuffers(1, &normalbuffer);
    //freeing geometry
    delete this->geometry;
    //freeing children
    for (AnimMesh *child : this->children)
    {
        delete child;
    }
    //freeing parent
    if (this->parent != nullptr)
    {
        delete this->parent;
    }
}

std::vector<AnimMesh *> AnimMesh::getChildren()
{
    return this->children;
}
AnimMesh *AnimMesh::addChild(AnimMesh *child)
{
    this->children.push_back(child);
    return child;
}
AnimMesh* AnimMesh::getParent()
{
    return this->parent;
}
AnimMesh *AnimMesh::setParent(AnimMesh *parent)
{
    this->parent = parent;
    return this;
}

void AnimMesh::draw(Camera *camera){
    //drawing children
    for (AnimMesh *child : this->children)
    {
        child->draw(camera);
    }
    //drawing self
    if(this->geometry == nullptr)
        return;
    //drawing self
    bindBuffers();
    setMVP(camera);
    // set uniforms
    GLuint ID = glGetUniformLocation(getShader(), "time");
    glUniform1f(ID, (float)glfwGetTime());
    ID = glGetUniformLocation(getShader(), "camPos");
    glUniform3f(ID, camera->position.x, camera->position.y, camera->position.z);

    glDrawArrays(GL_TRIANGLES, 0, this->getVertexCount());
    unbindBuffers();
}

void AnimMesh::genBuffers()
{
    if(this->geometry == nullptr)
        return;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vertexbuffer);    
    glGenBuffers(1, &uvbuffer);
    glGenBuffers(1, &normalbuffer);
    //filling buffers
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    if(this->geometry->vertices.size() > 0)
        glBufferData(GL_ARRAY_BUFFER, this->geometry->vertices.size() * sizeof(glm::vec3), this->geometry->vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    if(this->geometry->uvs.size() > 0)
        glBufferData(GL_ARRAY_BUFFER, this->geometry->uvs.size() * sizeof(glm::vec2), this->geometry->uvs.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    if(this->geometry->normals.size() > 0)
        glBufferData(GL_ARRAY_BUFFER, this->geometry->normals.size() * sizeof(glm::vec3), this->geometry->normals.data(), GL_STATIC_DRAW);
}
void AnimMesh::bindBuffers()
{
    if(this->geometry == nullptr)
        return;
    glUseProgram(this->getShader());
    glBindVertexArray(vao);
    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
        0,        // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,        // size vector 3d
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,        // stride
        (void *)0 // array buffer offset
    );
    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(
        1,        // attribute. No particular reason for 1, but must match the layout in the shader.
        2,        // size : vector 2d
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,        // stride
        (void *)0 // array buffer offset
    );
    // 3rd attribute buffer : normals
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(
        2,        // attribute. No particular reason for 2, but must match the layout in the shader.
        3,        // size : vector 3d
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,        // stride
        (void *)0 // array buffer offset
    );
    // Bind our texture in Texture Unit 0
    if(this->texture == nullptr)
        return;
    glActiveTexture(GL_TEXTURE0);                                // Hey OpenGL, we're about to give commands for texture sampler 0.
    glBindTexture(GL_TEXTURE_2D, this->texture->getTextureID()); // when the currently-active sampler samples from a 2D texture, this is the texture to use.
    glEnable(GL_TEXTURE_2D);                                     // the currently-active sampler should actually use the bound 2D texture.
}
void AnimMesh::attachShader(GLuint shaderID)
{
    // Use our shader
    this->shader = shaderID;
    // glUseProgram(shaderID);
}
GLuint AnimMesh::getShader()
{
    return this->shader;
}
void AnimMesh::unbindBuffers()
{
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    // Disable our texture
    glDisable(GL_TEXTURE_2D);
    // Unbind vao
    glBindVertexArray(0);
}
void AnimMesh::setMVP(GLuint shaderID, glm::mat4 MVP)
{
    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    GLuint MatrixID = glGetUniformLocation(shaderID, "MVP");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
}
// does almost everything needed with matrices
void AnimMesh::setMVP(Camera *camera)
{
    camera->updateProjection();
    camera->updateView();
    glm::mat4 MVP = camera->getProjection() * camera->getView() * this->getModelMatrix();
    this->setMVP(this->shader, MVP);
    GLuint MatrixID = glGetUniformLocation(this->getShader(), "model");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &this->getModelMatrix()[0][0]);
}

GLsizei AnimMesh::getVertexCount()
{
    return this->geometry->vertices.size();
}