
/*
 * 1
 */

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <common.hpp>

#define MAX_BONE_INFLUENCE 4

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
    Model(const std::string& path)
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

private:
    void LoadModel(const std::string& path)
    {
        Assimp::Importer import;
        const aiScene* scene
            = import.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cerr << "ERROR::ASSIMP::" << import.GetErrorString() << '\n';
            return;
        }

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

    Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;

            glm::vec3 vec3;

            // position
            vec3.x          = mesh->mVertices[i].x;
            vec3.y          = mesh->mVertices[i].y;
            vec3.z          = mesh->mVertices[i].z;
            vertex.Position = vec3;

            // normal
            if (mesh->HasNormals())
            {
                vec3.x        = mesh->mNormals[i].x;
                vec3.y        = mesh->mNormals[i].y;
                vec3.z        = mesh->mNormals[i].z;
                vertex.Normal = vec3;
            }

            // texture coordinates
            if (mesh->mTextureCoords[0])
            {
                glm::vec2 uv;

                // uv
                uv.x             = mesh->mTextureCoords[0][i].x;
                uv.y             = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = uv;

                if (mesh->HasTangentsAndBitangents())
                {
                    // tangent
                    vec3.x         = mesh->mTangents[i].x;
                    vec3.y         = mesh->mTangents[i].y;
                    vec3.z         = mesh->mTangents[i].z;
                    vertex.Tangent = vec3;

                    // bitangent
                    vec3.x           = mesh->mBitangents[i].x;
                    vec3.y           = mesh->mBitangents[i].y;
                    vec3.z           = mesh->mBitangents[i].z;
                    vertex.Bitangent = vec3;
                }
            }
            else
            {
                vertex.TexCoords = glm::vec2(0.f);
            }

            vertices.push_back(vertex);
        }

        // 处理索引
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

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

    unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false)
    {
        std::string filename = std::string(path);
        filename             = directory + '/' + filename;

        unsigned int textureID;
        glGenTextures(1, &textureID);

        std::cout << "load texture: " << filename << '\n';

        stbi_set_flip_vertically_on_load(true);
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
};
}; // namespace ModelLoading

int main()
{
    InitOpenGL init(Camera({ 0, 0, 10 }, { 0, 1, 0 }, { 0, 0, 0 }));
    auto window = init.GetWindow();
    ShaderProgram program("resources/02_03_TEST1.vs", "resources/02_03_TEST1.fs");

    ModelLoading::Model ourModel("resources/backpack.obj");

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.Use();

        program.SetUniformMat4("projection", init.GetProjectionMatrix());
        program.SetUniformMat4("view", init.GetViewMatrix());

        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model           = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        program.SetUniformMat4("model", model);

        ourModel.Draw(program);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
