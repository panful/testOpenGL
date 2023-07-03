#pragma once

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <common.hpp>

#define MAX_BONE_INFLUENCE 4

struct BoneInfo
{
    /*id is index in finalBoneMatrices*/
    int id;

    /*offset matrix transforms vertex from model space to bone space*/
    glm::mat4 offset;
};

class AssimpGLMHelpers
{
public:
    static inline glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from)
    {
        glm::mat4 to;
        // the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
        to[0][0] = from.a1;
        to[1][0] = from.a2;
        to[2][0] = from.a3;
        to[3][0] = from.a4;
        to[0][1] = from.b1;
        to[1][1] = from.b2;
        to[2][1] = from.b3;
        to[3][1] = from.b4;
        to[0][2] = from.c1;
        to[1][2] = from.c2;
        to[2][2] = from.c3;
        to[3][2] = from.c4;
        to[0][3] = from.d1;
        to[1][3] = from.d2;
        to[2][3] = from.d3;
        to[3][3] = from.d4;
        return to;
    }

    static inline glm::vec2 AiVec3ToGLMVec2(const aiVector3D& vec)
    {
        return glm::vec2(vec.x, vec.y);
    }

    static inline glm::vec3 AiVec3ToGLMVec3(const aiVector3D& vec)
    {
        return glm::vec3(vec.x, vec.y, vec.z);
    }

    static inline glm::quat GetGLMQuat(const aiQuaternion& pOrientation)
    {
        return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
    }
};

namespace ModelLoading {
struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;

    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
    // bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    // weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture
{
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh
{
public:
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
    std::vector<Texture> m_textures;

    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, std::vector<Texture> textures)
    {
        this->m_vertices = vertices;
        this->m_indices  = indices;
        this->m_textures = textures;

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        SetupMesh();
    }

    void Draw(ShaderProgram& shader)
    {
        // bind appropriate textures
        unsigned int diffuseNr  = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr   = 1;
        unsigned int heightNr   = 1;
        for (unsigned int i = 0; i < m_textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
                                              // retrieve texture number (the N in diffuse_textureN)
            std::string number;
            std::string name = m_textures[i].type;
            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++); // transfer unsigned int to string
            else if (name == "texture_normal")
                number = std::to_string(normalNr++);   // transfer unsigned int to string
            else if (name == "texture_height")
                number = std::to_string(heightNr++);   // transfer unsigned int to string

            // now set the sampler to the correct texture unit
            glUniform1i(glGetUniformLocation(shader.GetProgram(), (name + number).c_str()), i);
            // and finally bind the texture
            glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
        }

        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(m_indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }

private:
    unsigned int VAO, VBO, EBO;

    void SetupMesh()
    {
        // create buffers/arrays
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        // vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
        // ids
        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
        // weights
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));

        glBindVertexArray(0);
    }
};

class Model
{
public:
    /// @brief 加载模型
    /// @param path 模型的路径
    /// @param flip 是否上下翻转纹理
    Model(const std::string& path, bool flip = true)
        : m_flip(flip)
    {
        LoadModel(path);
    }

    void Draw(ShaderProgram& shader)
    {
        for (unsigned int i = 0; i < m_meshes.size(); i++)
        {
            m_meshes[i].Draw(shader);
        }
    }

    auto& GetBoneInfoMap()
    {
        return m_BoneInfoMap;
    }

    auto& GetBoneCount()
    {
        return m_BoneCounter;
    }

private:
    void LoadModel(const std::string& path)
    {
        Assimp::Importer import;
        const aiScene* scene = import.ReadFile(
            path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_OptimizeMeshes);

        if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
        {
            std::cerr << "ERROR::ASSIMP::" << import.GetErrorString() << '\n';
            return;
        }

        std::cout << "RootNode Number of Meshs:\t" << scene->mRootNode->mNumMeshes << '\n'
                  << "RootNode Number of Children:\t" << scene->mRootNode->mNumChildren << '\n';

        m_directory = path.substr(0, path.find_last_of('/'));
        ProcessNode(scene->mRootNode, scene);
    }

    void ProcessNode(aiNode* node, const aiScene* scene)
    {
        // 处理节点所有的网格（如果有的话）
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            m_meshes.push_back(ProcessMesh(mesh, scene));
        }

        // 接下来对它的子节点重复这一过程
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], scene);
        }
    }

    void SetVertexBoneDataToDefault(Vertex& vertex)
    {
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
        {
            vertex.m_BoneIDs[i] = -1;
            vertex.m_Weights[i] = 0.0f;
        }
    }

    void SetVertexBoneData(Vertex& vertex, int boneID, float weight)
    {
        for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
        {
            if (vertex.m_BoneIDs[i] < 0)
            {
                vertex.m_Weights[i] = weight;
                vertex.m_BoneIDs[i] = boneID;
                break;
            }
        }
    }

    void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
    {
        auto& boneInfoMap = m_BoneInfoMap;
        auto& boneCount   = m_BoneCounter;

        static int index { 0 };
        std::cout << "Mesh " << index++ << " Number of Bones: " << mesh->mNumBones << '\n';

        for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
        {
            int boneID           = -1;
            std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
            if (boneInfoMap.find(boneName) == boneInfoMap.end())
            {
                BoneInfo newBoneInfo;
                newBoneInfo.id        = boneCount;
                newBoneInfo.offset    = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
                boneInfoMap[boneName] = newBoneInfo;
                boneID                = boneCount;
                boneCount++;
            }
            else
            {
                boneID = boneInfoMap[boneName].id;
            }

            assert(boneID != -1);

            auto weights   = mesh->mBones[boneIndex]->mWeights;
            int numWeights = mesh->mBones[boneIndex]->mNumWeights;

            if (numWeights > 0)
            {
                // 骨骼的名称，骨骼影响的顶点个数
                std::cout << "Bone " << boneIndex << " Bone Name: " << boneName << "\tNumber of vertices affected: " << numWeights << '\n';
            }

            // 提取将受此骨骼影响的顶点以及权重
            for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
            {
                int vertexId = weights[weightIndex].mVertexId;
                float weight = weights[weightIndex].mWeight;

                assert(vertexId <= vertices.size());

                SetVertexBoneData(vertices[vertexId], boneID, weight);
            }
        }
    }

    Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, bool animation = true)
    {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::vector<Texture> textures;

        // 处理材质
        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

            // 1. diffuse maps
            std::vector<Texture> diffuseMaps = LoadTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            // 2. specular maps
            std::vector<Texture> specularMaps = LoadTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
            // 3. normal maps
            std::vector<Texture> normalMaps = LoadTextures(material, aiTextureType_HEIGHT, "texture_normal");
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
            // 4. height maps
            std::vector<Texture> heightMaps = LoadTextures(material, aiTextureType_AMBIENT, "texture_height");
            textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        }

        static int index { 0 };
        std::cout << "Mesh " << index++ << ":\tNumber of Vertices: " << mesh->mNumVertices << "\tNumber of Indices: " << mesh->mNumFaces << '\n';

        aiVector3D minCoords(FLT_MAX, FLT_MAX, FLT_MAX);
        aiVector3D maxCoords(-FLT_MAX, -FLT_MAX, -FLT_MAX);

        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            // 求AABB包围盒，assimp自带的AABB返回的都是0
            aiVector3D vertexAABB = mesh->mVertices[i];
            minCoords.x           = std::min(minCoords.x, vertexAABB.x);
            minCoords.y           = std::min(minCoords.y, vertexAABB.y);
            minCoords.z           = std::min(minCoords.z, vertexAABB.z);

            maxCoords.x = std::max(maxCoords.x, vertexAABB.x);
            maxCoords.y = std::max(maxCoords.y, vertexAABB.y);
            maxCoords.z = std::max(maxCoords.z, vertexAABB.z);

            Vertex vertex;
            SetVertexBoneDataToDefault(vertex);
            vertex.Position = AssimpGLMHelpers::AiVec3ToGLMVec3(mesh->mVertices[i]);
            vertex.Normal   = AssimpGLMHelpers::AiVec3ToGLMVec3(mesh->mNormals[i]);

            // texture coordinates
            if (mesh->HasTextureCoords(0))
            {
                // uv
                vertex.TexCoords = AssimpGLMHelpers::AiVec3ToGLMVec2(mesh->mTextureCoords[0][i]);

                if (mesh->HasTangentsAndBitangents())
                {
                    // tangent
                    vertex.Tangent = AssimpGLMHelpers::AiVec3ToGLMVec3(mesh->mTangents[i]);
                    // bitangent
                    vertex.Bitangent = AssimpGLMHelpers::AiVec3ToGLMVec3(mesh->mBitangents[i]);
                }
            }
            else
            {
                vertex.TexCoords = glm::vec2(0.f);
            }

            vertices.push_back(vertex);
        }

        std::cout << "AABB Max: " << maxCoords.x << ',' << maxCoords.y << ',' << maxCoords.z << "\tAABB Min: " << minCoords.x << ',' << minCoords.y
                  << ',' << minCoords.z << '\n';

        // 处理索引
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        ExtractBoneWeightForVertices(vertices, mesh, scene);

        return Mesh(vertices, indices, textures);
    }

    std::vector<Texture> LoadTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName)
    {
        std::vector<Texture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);

            // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            bool skip = false;
            for (unsigned int j = 0; j < m_textures.size(); j++)
            {
                if (std::strcmp(m_textures[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(m_textures[j]);
                    skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }

            if (!skip)
            {
                // if texture hasn't been loaded already, load it
                Texture texture;
                texture.id   = TextureFromFile(str.C_Str(), this->m_directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                m_textures.push_back(texture); // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
            }
        }
        return textures;
    }

    uint32_t TextureFromFile(const char* path, const std::string& directory, bool gamma = false)
    {
        std::string filename = std::string(path);
        filename             = directory + '/' + filename;

        unsigned int textureID;
        glGenTextures(1, &textureID);

        std::cout << "load texture: " << filename << '\n';

        stbi_set_flip_vertically_on_load(m_flip);

        int width, height, nrComponents;
        unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }

        return textureID;
    }

private:
    std::string m_directory;
    std::vector<Mesh> m_meshes;
    std::vector<Texture> m_textures;
    std::map<std::string, BoneInfo> m_BoneInfoMap;
    int m_BoneCounter { 0 };
    bool m_flip { true };
};
}; // namespace ModelLoading