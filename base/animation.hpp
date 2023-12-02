#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <assimp/matrix4x4.h>
#include <assimp/quaternion.h>
#include <assimp/vector3.h>

#include <model.hpp>

namespace Animation {

// 三个Key用来保存每一个关键帧的数据
// 包括一个平移\旋转\缩放的值和一个时间戳
struct KeyPosition
{
    glm::vec3 position;
    float timeStamp;
};

struct KeyRotation
{
    glm::quat orientation;
    float timeStamp;
};

struct KeyScale
{
    glm::vec3 scale;
    float timeStamp;
};

// 骨骼的父子关系层次结构
struct AssimpNodeData
{
    glm::mat4 transformation;
    std::string name;
    int childrenCount;
    std::vector<AssimpNodeData> children;
};

//-------------------------------------------------------------------------------------------------------------

class Bone
{
public:
    Bone(const std::string& name, int ID, const aiNodeAnim* channel)
        : m_Name(name)
        , m_ID(ID)
        , m_LocalTransform(1.0f)
    {
        m_NumPositions = channel->mNumPositionKeys;
        for (int positionIndex = 0; positionIndex < m_NumPositions; ++positionIndex)
        {
            auto aiPosition = channel->mPositionKeys[positionIndex].mValue;
            auto timeStamp  = channel->mPositionKeys[positionIndex].mTime;
            KeyPosition data;
            data.position  = AssimpGLMHelpers::AiVec3ToGLMVec3(aiPosition);
            data.timeStamp = static_cast<float>(timeStamp);
            m_Positions.push_back(data);
        }

        m_NumRotations = channel->mNumRotationKeys;
        for (int rotationIndex = 0; rotationIndex < m_NumRotations; ++rotationIndex)
        {
            auto aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
            auto timeStamp     = channel->mRotationKeys[rotationIndex].mTime;
            KeyRotation data;
            data.orientation = AssimpGLMHelpers::GetGLMQuat(aiOrientation);
            data.timeStamp   = static_cast<float>(timeStamp);
            m_Rotations.push_back(data);
        }

        m_NumScalings = channel->mNumScalingKeys;
        for (int keyIndex = 0; keyIndex < m_NumScalings; ++keyIndex)
        {
            auto scale     = channel->mScalingKeys[keyIndex].mValue;
            auto timeStamp = channel->mScalingKeys[keyIndex].mTime;
            KeyScale data;
            data.scale     = AssimpGLMHelpers::AiVec3ToGLMVec3(scale);
            data.timeStamp = static_cast<float>(timeStamp);
            m_Scales.push_back(data);
        }

        std::cout << "Bone " << ID << " positions: " << m_NumPositions << "\tRotations: " << m_NumRotations << "\tScalings: " << m_NumScalings
                  << "\n";
    }

    void Update(float animationTime)
    {
        glm::mat4 translation = InterpolatePosition(animationTime);
        glm::mat4 rotation    = InterpolateRotation(animationTime);
        glm::mat4 scale       = InterpolateScaling(animationTime);
        m_LocalTransform      = translation * rotation * scale;
    }

    glm::mat4 GetLocalTransform()
    {
        return m_LocalTransform;
    }

    std::string GetBoneName() const
    {
        return m_Name;
    }

    int GetBoneID()
    {
        return m_ID;
    }

    // 找到当前时间对应的关键帧的index
    int GetPositionIndex(float animationTime)
    {
        for (int index = 0; index < m_NumPositions - 1; ++index)
        {
            if (animationTime < m_Positions[index + 1].timeStamp)
                return index;
        }
        assert(0);
        return 0;
    }

    int GetRotationIndex(float animationTime)
    {
        for (int index = 0; index < m_NumRotations - 1; ++index)
        {
            if (animationTime < m_Rotations[index + 1].timeStamp)
                return index;
        }
        assert(0);
        return 0;
    }

    int GetScaleIndex(float animationTime)
    {
        for (int index = 0; index < m_NumScalings - 1; ++index)
        {
            if (animationTime < m_Scales[index + 1].timeStamp)
                return index;
        }
        assert(0);
        return 0;
    }

private:
    float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
    {
        float scaleFactor  = 0.0f;
        float midWayLength = animationTime - lastTimeStamp;
        float framesDiff   = nextTimeStamp - lastTimeStamp;
        scaleFactor        = midWayLength / framesDiff;
        return scaleFactor;
    }

    glm::mat4 InterpolatePosition(float animationTime)
    {
        if (1 == m_NumPositions)
        {
            return glm::translate(glm::mat4(1.0f), m_Positions[0].position);
        }

        int p0Index = GetPositionIndex(animationTime);
        int p1Index = p0Index + 1;

        // 通过对两个相邻的关键帧做插值处理，生成一个平移矩阵，旋转和缩放类似
        float scaleFactor       = GetScaleFactor(m_Positions[p0Index].timeStamp, m_Positions[p1Index].timeStamp, animationTime);
        glm::vec3 finalPosition = glm::mix(m_Positions[p0Index].position, m_Positions[p1Index].position, scaleFactor);
        return glm::translate(glm::mat4(1.0f), finalPosition);
    }

    glm::mat4 InterpolateRotation(float animationTime)
    {
        if (1 == m_NumRotations)
        {
            auto rotation = glm::normalize(m_Rotations[0].orientation);
            return glm::toMat4(rotation);
        }

        int p0Index             = GetRotationIndex(animationTime);
        int p1Index             = p0Index + 1;
        float scaleFactor       = GetScaleFactor(m_Rotations[p0Index].timeStamp, m_Rotations[p1Index].timeStamp, animationTime);
        glm::quat finalRotation = glm::slerp(m_Rotations[p0Index].orientation, m_Rotations[p1Index].orientation, scaleFactor);
        finalRotation           = glm::normalize(finalRotation);
        return glm::toMat4(finalRotation);
    }

    glm::mat4 InterpolateScaling(float animationTime)
    {
        if (1 == m_NumScalings)
        {
            return glm::scale(glm::mat4(1.0f), m_Scales[0].scale);
        }

        int p0Index          = GetScaleIndex(animationTime);
        int p1Index          = p0Index + 1;
        float scaleFactor    = GetScaleFactor(m_Scales[p0Index].timeStamp, m_Scales[p1Index].timeStamp, animationTime);
        glm::vec3 finalScale = glm::mix(m_Scales[p0Index].scale, m_Scales[p1Index].scale, scaleFactor);
        return glm::scale(glm::mat4(1.0f), finalScale);
    }

private:
    std::vector<KeyPosition> m_Positions;
    std::vector<KeyRotation> m_Rotations;
    std::vector<KeyScale> m_Scales;
    int m_NumPositions;
    int m_NumRotations;
    int m_NumScalings;

    glm::mat4 m_LocalTransform;
    std::string m_Name;
    int m_ID;
};

//------------------------------------------------------------------------------------------------

class Animation
{
public:
    Animation(const std::string& animationPath, ModelLoading::Model* model)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
        assert(scene && scene->mRootNode);
        auto animation                   = scene->mAnimations[0];
        m_Duration                       = static_cast<float>(animation->mDuration);
        m_TicksPerSecond                 = static_cast<int>(animation->mTicksPerSecond);
        aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
        globalTransformation             = globalTransformation.Inverse();
        ReadHierarchyData(m_RootNode, scene->mRootNode);
        ReadMissingBones(animation, *model);
    }

    Bone* FindBone(const std::string& name)
    {
        auto iter = std::find_if(m_Bones.begin(), m_Bones.end(), [&](const Bone& Bone) { return Bone.GetBoneName() == name; });
        if (iter == m_Bones.end())
            return nullptr;
        else
            return &(*iter);
    }

    // 每秒滴答的次数：1000，就是每秒由多少毫秒构成
    inline float GetTicksPerSecond()
    {
        return static_cast<float>(m_TicksPerSecond);
    }

    // 动画的持续时间（毫秒）
    inline float GetDuration()
    {
        return m_Duration;
    }

    inline const AssimpNodeData& GetRootNode()
    {
        return m_RootNode;
    }

    inline const std::map<std::string, BoneInfo>& GetBoneIDMap()
    {
        return m_BoneInfoMap;
    }

private:
    void ReadMissingBones(const aiAnimation* animation, ModelLoading::Model& model)
    {
        int size = animation->mNumChannels;

        auto& boneInfoMap = model.GetBoneInfoMap();
        auto& boneCount   = model.GetBoneCount();

        // 某些模型可能会缺失某些骨骼，此处是为了保证将所有骨骼信息读取
        for (int i = 0; i < size; i++)
        {
            // mChannels包含参与动画的所有骨骼和关键帧
            auto channel         = animation->mChannels[i];
            std::string boneName = channel->mNodeName.data;

            if (boneInfoMap.find(boneName) == boneInfoMap.end())
            {
                boneInfoMap[boneName].id = boneCount;
                boneCount++;
            }
            m_Bones.push_back(Bone(channel->mNodeName.data, boneInfoMap[channel->mNodeName.data].id, channel));
        }

        m_BoneInfoMap = boneInfoMap;
    }

    // 骨骼的父子关系，比如右肩是右二头肌、前臂、手和手指的父亲，即右肩移动时，它的四个孩子节点也应该移动
    void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src)
    {
        assert(src);

        dest.name           = src->mName.data;
        dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
        dest.childrenCount  = src->mNumChildren;

        for (uint32_t i = 0; i < src->mNumChildren; i++)
        {
            AssimpNodeData newData;
            ReadHierarchyData(newData, src->mChildren[i]);
            dest.children.push_back(newData);
        }
    }

private:
    float m_Duration;
    int m_TicksPerSecond;
    std::vector<Bone> m_Bones;
    AssimpNodeData m_RootNode;
    std::map<std::string, BoneInfo> m_BoneInfoMap;
};

class Animator
{
public:
    Animator(Animation* animation)
    {
        m_CurrentTime      = 0.0;
        m_CurrentAnimation = animation;

        // 此处的100只要比Animation::m_BoneInfoMap的size大就可以，即比骨骼的个数大
        m_FinalBoneMatrices.reserve(100);

        for (int i = 0; i < 100; i++)
        {
            m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
        }
    }

    void UpdateAnimation(float dt)
    {
        m_DeltaTime = dt;
        if (m_CurrentAnimation)
        {
            // 将dt(秒)转换为毫秒
            m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
            // 取余，当前时刻在动画开始到结束之间所处的位置
            m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
            CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
        }
    }

    void PlayAnimation(Animation* pAnimation)
    {
        m_CurrentAnimation = pAnimation;
        m_CurrentTime      = 0.0f;
    }

    void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
    {
        std::string nodeName    = node->name;
        glm::mat4 nodeTransform = node->transformation;

        Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

        if (Bone)
        {
            Bone->Update(m_CurrentTime);
            nodeTransform = Bone->GetLocalTransform();
        }

        glm::mat4 globalTransformation = parentTransform * nodeTransform;

        auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
        if (boneInfoMap.find(nodeName) != boneInfoMap.end())
        {
            int index                  = boneInfoMap[nodeName].id;
            glm::mat4 offset           = boneInfoMap[nodeName].offset;
            m_FinalBoneMatrices[index] = globalTransformation * offset;
        }

        // 根据骨骼的父子关系，依次从根节点将变换矩阵应用到叶子节点
        for (int i = 0; i < node->childrenCount; i++)
        {
            CalculateBoneTransform(&node->children[i], globalTransformation);
        }
    }

    std::vector<glm::mat4> GetFinalBoneMatrices()
    {
        return m_FinalBoneMatrices;
    }

private:
    std::vector<glm::mat4> m_FinalBoneMatrices;
    Animation* m_CurrentAnimation;
    float m_CurrentTime;
    float m_DeltaTime;
};
}; // namespace Animation