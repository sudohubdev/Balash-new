#include "AnimMesh.hpp"


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
        std::vector <Vertex> vertices = std::vector<Vertex>();
        for(auto vertex : animMesh->getGeometry()->vertices){
            Vertex vertexstruct;
            animMesh->SetVertexBoneDataToDefault(vertexstruct);
            vertexstruct.Position = vertex;
            vertices.push_back(vertexstruct);
        }
        animMesh->ExtractBoneWeightForVertices(vertices,mesh,scene);
        animMesh->boneIDs = std::vector<Tetraint>();
        animMesh->weights = std::vector<glm::vec4>();
        for(auto vertex : vertices){
            Tetraint tetraint = *(Tetraint*)&vertex.m_BoneIDs;
            
            animMesh->boneIDs.push_back(tetraint);
            animMesh->weights.push_back(glm::vec4(vertex.m_Weights[0],vertex.m_Weights[1],vertex.m_Weights[2],vertex.m_Weights[3])); 
        }
        animMesh->genBuffers();
        animMesh->attachShader(ANIMSHADER);
        animMesh->setTexture(new Texture("assets/Vampire_diffuse.png"));
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
    this->geometry = nullptr;
}
AnimMesh::AnimMesh(string path)
{
    this->children = std::vector<AnimMesh *>();
    this->parent = nullptr;
    this->geometry = nullptr;

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


std::map<string, BoneInfo> AnimMesh::GetBoneInfoMap() { 
    return m_BoneInfoMap; 
}
int& AnimMesh::GetBoneCount() { 
    return m_BoneCounter; 
}


void AnimMesh::SetVertexBoneData(Vertex& vertex, int boneID, float weight)
{
    for (int i = 0; i < MAX_BONE_WEIGHTS; ++i)
    {
        if (vertex.m_BoneIDs[i] < 0)
        {
            vertex.m_Weights[i] = weight;
            vertex.m_BoneIDs[i] = boneID;
            break;
        }
    }
}

void AnimMesh::SetVertexBoneDataToDefault(Vertex& vertex)
{
    for (int i = 0; i < MAX_BONE_WEIGHTS; i++)
    {
        vertex.m_BoneIDs[i] = -1;
        vertex.m_Weights[i] = 0.0f;
    }
}

void AnimMesh::ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
{
    for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
    {
        int boneID = -1;
        std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
        if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end())
        {
            BoneInfo newBoneInfo;
            newBoneInfo.id = m_BoneCounter;
            newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(
                mesh->mBones[boneIndex]->mOffsetMatrix);
            m_BoneInfoMap[boneName] = newBoneInfo;
            boneID = m_BoneCounter;
            m_BoneCounter++;
        }
        else
        {
            boneID = m_BoneInfoMap[boneName].id;
        }
        assert(boneID != -1);
        auto weights = mesh->mBones[boneIndex]->mWeights;
        int numWeights = mesh->mBones[boneIndex]->mNumWeights;

        for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
        {
            int vertexId = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;
            assert(vertexId <= vertices.size());
            SetVertexBoneData(vertices[vertexId], boneID, weight);
        }
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


    for (int i = 0; i < transforms.size(); ++i){
        std::string name = "finalBonesMatrices[" + std::to_string(i) + "]";
        GLuint MatrixID = glGetUniformLocation(getShader(), name.c_str());
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &transforms[i][0][0]);
    }


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glDrawElements(GL_TRIANGLES, this->geometry->indices.size(), GL_UNSIGNED_INT, (void *)0);
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
    glGenBuffers(1, &elementbuffer);
    glGenBuffers(1, &bonebuffer);
    glGenBuffers(1, &weightbuffer);
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
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    if(this->geometry->indices.size() > 0)
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->geometry->indices.size() * sizeof(GLuint), this->geometry->indices.data(), GL_STATIC_DRAW);
    //
    glBindBuffer(GL_ARRAY_BUFFER, bonebuffer);//FIXME:
    glBufferData(GL_ARRAY_BUFFER, this->boneIDs.size() * sizeof(int) * 4, this->boneIDs.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, weightbuffer);
    glBufferData(GL_ARRAY_BUFFER, this->weights.size() * sizeof(glm::vec4), this->weights.data(), GL_DYNAMIC_DRAW);
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

    //bones
    // ids
    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, bonebuffer);
    glBufferData(GL_ARRAY_BUFFER, this->boneIDs.size() * sizeof(int) * 4, this->boneIDs.data(), GL_DYNAMIC_DRAW);
    glVertexAttribIPointer(
        3,        // attribute. No particular reason for 3, but must match the layout in the shader.
        4,        // size : vector 4d of ints
        GL_INT, // type
        0,        // stride
        (void *)0 // array buffer offset
    );
    // weights
    glEnableVertexAttribArray(4);
    glBindBuffer(GL_ARRAY_BUFFER, weightbuffer);
    glBufferData(GL_ARRAY_BUFFER, this->weights.size() * sizeof(glm::vec4), this->weights.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(
        4,        // attribute. No particular reason for 4, but must match the layout in the shader.
        4,        // size : vector 4d
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
    //
    GLuint MatrixID = glGetUniformLocation(this->getShader(), "model");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &this->getModelMatrix()[0][0]);
    MatrixID = glGetUniformLocation(this->getShader(), "view");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &camera->getView()[0][0]);
    MatrixID = glGetUniformLocation(this->getShader(), "projection");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &camera->getProjection()[0][0]);
}

GLsizei AnimMesh::getVertexCount()
{
    return this->geometry->vertices.size();
}