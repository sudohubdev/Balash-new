#include "../../libs.hpp"
#include "AnimMesh.hpp"
#include "Bone.hpp"

#ifndef ANIMATION_HPP
#define ANIMATION_HPP

struct AssimpNodeData
{
    glm::mat4 transformation;
    std::string name;
    int childrenCount;
    std::vector<AssimpNodeData> children;
};

class Animation
{
public:
    Animation() = default;

    Animation(const std::string& animationPath, AnimMesh* model);

    ~Animation(){  
        m_Bones.clear();
        m_BoneInfoMap.clear();
    }

    Bone* FindBone(const std::string& name);
	
    inline float GetTicksPerSecond() { return m_TicksPerSecond; }

    inline float GetDuration() { return m_Duration;}

    inline const AssimpNodeData& GetRootNode() { return m_RootNode; }

    inline const std::map<std::string,BoneInfo>& GetBoneIDMap() { return m_BoneInfoMap; }

private:
    void ReadMissingBones(const aiAnimation* animation, AnimMesh& model);

    void ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src);
    float m_Duration;
    int m_TicksPerSecond;
    std::vector<Bone> m_Bones;
    AssimpNodeData m_RootNode;
    std::map<std::string, BoneInfo> m_BoneInfoMap;
};

#endif