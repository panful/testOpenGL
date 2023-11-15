#pragma once

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION

// clang-format off
#define HAS_STB_IMAGE
#include "tiny_gltf.h"
#include "common.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
// clang-format on

#include <variant>

struct Primitive
{
    uint32_t VAO { 0 };
    int indexCount { 0 };
    uint32_t indexType { 0 };
    int material { 0 };
    uint32_t mode { 0 };
    glm::mat4 modelMat { glm::mat4(1.f) };
};

class ModelglTFLoading
{
public:
    ModelglTFLoading(const std::filesystem::path& filename)
    {
        LoadModel(filename);
    }

    void Draw(ShaderProgram& program)
    {
        for (const auto& primitive : m_primitives)
        {
            if (primitive.material > -1)
            {
                std::visit(
                    [this, &program](auto&& var)
                    {
                        using T = std::decay_t<decltype(var)>;
                        if constexpr (std::is_same_v<T, int>)
                        {
                            if (auto texIndex = m_textures[var]; texIndex > -1)
                            {
                                glBindTexture(GL_TEXTURE_2D, m_glTextureIDs[texIndex]);
                            }
                        }
                        else if constexpr (std::is_same_v<T, glm::vec4>)
                        {
                            program.SetUniform4fv("uColor", var);
                        }
                    },
                    m_materials[primitive.material]);
            }
            else
            {
                program.SetUniform4f("uColor", 1.f, 0.f, 0.f, 1.f);
            }

            program.SetUniformMat4("model", primitive.modelMat);

            glBindVertexArray(primitive.VAO);
            glDrawElements(primitive.mode, primitive.indexCount, primitive.indexType, 0);
            glBindVertexArray(0);
        }
    }

private:
    void LoadModel(const std::filesystem::path& filename)
    {
        tinygltf::TinyGLTF loader;

        std::string err;
        std::string warn;

        bool res { false };
        if (filename.has_extension())
        {
            auto ex = filename.extension();
            if (filename.extension() == ".glb")
            {
                res = loader.LoadBinaryFromFile(&m_model, &err, &warn, filename.string());
            }
            if (filename.extension() == ".gltf")
            {
                res = loader.LoadASCIIFromFile(&m_model, &err, &warn, filename.string());
            }
        }

        if (!warn.empty())
        {
            std::cout << "WARN: " << warn << std::endl;
        }

        if (!err.empty())
        {
            std::cout << "ERR: " << err << std::endl;
        }

        if (!res)
        {
            std::cout << "Failed to load glTF: " << filename << std::endl;
            return;
        }

        std::cout << "scenes size: " << m_model.scenes.size() << std::endl;
        std::cout << "nodes size: " << m_model.nodes.size() << std::endl;

        LoadImages();
        LoadTextures();
        LoadMaterials();

        for (auto& scene : m_model.scenes)
        {
            for (auto node : scene.nodes)
            {
                const auto glTFNode = m_model.nodes[static_cast<size_t>(node)];
                LoadNode(glTFNode, m_model);
            }
        }
    }

    void LoadNode(const tinygltf::Node& inputNode, const tinygltf::Model& model, const glm::mat4& mat = glm::mat4(1.f))
    {
        std::cout << "node: " << inputNode.name << std::endl;
        glm::mat4 tmpMat = mat;

        if (inputNode.translation.size() == 3)
        {
            tmpMat = glm::translate(tmpMat, glm::vec3(glm::make_vec3(inputNode.translation.data())));
        }
        if (inputNode.rotation.size() == 4)
        {
            glm::quat quat = glm::make_quat(inputNode.rotation.data());
            tmpMat *= glm::mat4(quat);
        }
        if (inputNode.scale.size() == 3)
        {
            tmpMat = glm::scale(tmpMat, glm::vec3(glm::make_vec3(inputNode.scale.data())));
        }
        if (inputNode.matrix.size() == 16)
        {
            tmpMat = glm::make_mat4x4(inputNode.matrix.data());
        }

        for (auto child : inputNode.children)
        {
            LoadNode(model.nodes[child], model, tmpMat);
        }

        // vertices indices
        if (inputNode.mesh > -1)
        {
            for (auto& primitive : model.meshes[inputNode.mesh].primitives)
            {
                unsigned int VAO { 0 };
                glGenVertexArrays(1, &VAO);
                glBindVertexArray(VAO);

                Primitive tmpPrimitive {};
                tmpPrimitive.VAO = VAO;

                // vertices
                {
                    uint32_t VBO[3] {};
                    glGenBuffers(3, VBO);
                    size_t vertexCount { 0 };

                    const void* pos_buffer { nullptr };
                    if (primitive.attributes.contains("POSITION"))
                    {
                        auto accessor   = model.accessors[primitive.attributes.at("POSITION")];
                        auto bufferView = model.bufferViews[accessor.bufferView];
                        pos_buffer      = &model.buffers[bufferView.buffer].data[accessor.byteOffset + bufferView.byteOffset];
                        vertexCount     = accessor.count;

                        std::cout << "vertices size: " << vertexCount << '\n';
                    }
                    else
                    {
                        std::cout << "position is empty\n";
                    }
                    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
                    glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(float) * 3, pos_buffer, GL_STATIC_DRAW);
                    glEnableVertexAttribArray(0);
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

                    const void* normal_buffer { nullptr };
                    if (primitive.attributes.contains("NORMAL"))
                    {
                        auto accessor   = model.accessors[primitive.attributes.at("NORMAL")];
                        auto bufferView = model.bufferViews[accessor.bufferView];
                        normal_buffer   = &model.buffers[bufferView.buffer].data[accessor.byteOffset + bufferView.byteOffset];
                        if (accessor.count != vertexCount)
                        {
                            std::cout << "position and normal are not equal\n";
                        }
                    }
                    else
                    {
                        std::vector<float> buf(vertexCount * 3);
                        normal_buffer = buf.data();
                    }
                    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
                    glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(float) * 3, normal_buffer, GL_STATIC_DRAW);
                    glEnableVertexAttribArray(1);
                    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

                    const void* texcoord_buffer { nullptr };
                    if (primitive.attributes.contains("TEXCOORD_0"))
                    {
                        auto accessor   = model.accessors[primitive.attributes.at("TEXCOORD_0")];
                        auto bufferView = model.bufferViews[accessor.bufferView];
                        texcoord_buffer = &model.buffers[bufferView.buffer].data[accessor.byteOffset + bufferView.byteOffset];
                        if (accessor.count != vertexCount)
                        {
                            std::cout << "position and texcooed are not equal\n";
                        }
                    }
                    else
                    {
                        std::vector<float> buf(vertexCount * 2);
                        texcoord_buffer = buf.data();
                    }
                    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
                    glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(float) * 2, texcoord_buffer, GL_STATIC_DRAW);
                    glEnableVertexAttribArray(2);
                    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
                }

                // indices
                {
                    auto accessor   = model.accessors[primitive.indices];
                    auto bufferView = model.bufferViews[accessor.bufferView];

                    if (TINYGLTF_TARGET_ELEMENT_ARRAY_BUFFER == bufferView.target)
                    {
                        auto buffer             = model.buffers[bufferView.buffer];
                        uint32_t indexCount     = static_cast<uint32_t>(accessor.count);
                        tmpPrimitive.indexCount = indexCount;

                        std::cout << "indices size: " << indexCount << '\n';

                        uint32_t EBO { 0 };
                        glGenBuffers(1, &EBO);
                        switch (accessor.componentType)
                        {
                        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
                        {
                            auto buf = &buffer.data[accessor.byteOffset + bufferView.byteOffset];
                            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(uint32_t), buf, GL_STATIC_DRAW);
                            tmpPrimitive.indexType = GL_UNSIGNED_INT;
                        }
                        break;
                        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
                        {
                            auto buf = &buffer.data[accessor.byteOffset + bufferView.byteOffset];
                            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(uint16_t), buf, GL_STATIC_DRAW);
                            tmpPrimitive.indexType = GL_UNSIGNED_SHORT;
                        }
                        break;
                        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
                        {
                            auto buf = &buffer.data[accessor.byteOffset + bufferView.byteOffset];
                            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(uint8_t), buf, GL_STATIC_DRAW);
                            tmpPrimitive.indexType = GL_UNSIGNED_BYTE;
                        }
                        break;
                        default:
                            break;
                        }
                    }
                }

                if (TINYGLTF_MODE_TRIANGLES != primitive.mode)
                {
                    std::cout << "primitive is not triangle";
                }
                tmpPrimitive.material = primitive.material;
                tmpPrimitive.mode     = GL_TRIANGLES;
                tmpPrimitive.modelMat = tmpMat;

                m_primitives.emplace_back(tmpPrimitive);

                glBindVertexArray(0);
            }
        }
    }

    void LoadTextures()
    {
        for (auto texture : m_model.textures)
        {
            m_textures.emplace_back(texture.source);
        }
    }

    void LoadMaterials()
    {
        for (auto& material : m_model.materials)
        {
            if (material.values.contains("baseColorFactor"))
            {
                m_materials.emplace_back(glm::make_vec4(material.values.at("baseColorFactor").ColorFactor().data()));
            }
            else if (material.values.contains("baseColorTexture"))
            {
                m_materials.emplace_back(material.values.at("baseColorTexture").TextureIndex());
            }
            else
            {
                // 如果没有默认的颜色或纹理，设置图元为绿色
                m_materials.emplace_back(glm::vec4(0.f, 1.f, 0.f, 1.f));
            }
        }
    }

    void LoadImages()
    {
        std::vector<uint32_t> images;
        for (auto& image : m_model.images)
        {
            uint32_t texture { 0 };
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            if (image.component == 3)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.image.data());
            }
            else if (image.component == 4)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.image.data());
            }
            else
            {
                std::cout << "image component error: " << image.component << '\n';
            }
            glGenerateMipmap(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);

            m_glTextureIDs.emplace_back(texture);
        }
    }

private:
    std::vector<Primitive> m_primitives;
    std::vector<std::variant<glm::vec4, int>> m_materials;
    std::vector<int> m_textures;
    std::vector<uint32_t> m_glTextureIDs;
    tinygltf::Model m_model;
};
