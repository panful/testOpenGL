#pragma once

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION

#define HAS_STB_IMAGE

#include "tiny_gltf.h"
#include "common.hpp"

class ModelglTFLoading
{
public:
    ModelglTFLoading(const std::filesystem::path& filename)
    {
        LoadModel(filename);
    }

    void Draw()
    {
        for (auto [vao, count_type] : m_vaos)
        {
            glBindVertexArray(vao);
            glDrawElements(GL_TRIANGLES, std::get<0>(count_type), std::get<1>(count_type), 0);
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

        // LoadImage(model);
        // LoadTexture(model);
        // LoadMaterial(model);

        for (auto& scene : m_model.scenes)
        {
            for (auto node : scene.nodes)
            {
                const auto glTFNode = m_model.nodes[static_cast<size_t>(node)];
                LoadNode(glTFNode, m_model);
            }
        }
    }

    void LoadNode(const tinygltf::Node& inputNode, const tinygltf::Model& model)
    {
        for (auto child : inputNode.children)
        {
            LoadNode(model.nodes[child], model);
        }

        // vertices indices
        if (inputNode.mesh > -1)
        {
            auto size = model.meshes[inputNode.mesh].primitives.size();
            for (auto& primitive : model.meshes[inputNode.mesh].primitives)
            {
                unsigned int VBO, VAO, EBO;
                glGenVertexArrays(1, &VAO);
                glGenBuffers(1, &VBO);
                glGenBuffers(1, &EBO);

                glBindVertexArray(VAO);

                // vertices
                {
                    if (primitive.attributes.contains("POSITION"))
                    {
                        auto accessor    = model.accessors[primitive.attributes.at("POSITION")];
                        auto bufferView  = model.bufferViews[accessor.bufferView];
                        auto buf         = &model.buffers[bufferView.buffer].data[accessor.byteOffset + bufferView.byteOffset];
                        auto vertexCount = accessor.count;

                        glBindBuffer(GL_ARRAY_BUFFER, VBO);
                        glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(float) * 3, buf, GL_STATIC_DRAW);

                        std::cout << "vertices size: " << vertexCount << '\n';
                    }

                    if (primitive.attributes.contains("NORMAL"))
                    {
                        auto accessor      = model.accessors[primitive.attributes.at("NORMAL")];
                        auto bufferView    = model.bufferViews[accessor.bufferView];
                        auto normalsBuffer = model.buffers[bufferView.buffer].data[accessor.byteOffset + bufferView.byteOffset];
                    }

                    if (primitive.attributes.contains("TEXCOORD_0"))
                    {
                        auto accessor        = model.accessors[primitive.attributes.at("TEXCOORD_0")];
                        auto bufferView      = model.bufferViews[accessor.bufferView];
                        auto texCoordsBuffer = model.buffers[bufferView.buffer].data[accessor.byteOffset + bufferView.byteOffset];
                    }

                    // 合并顶点属性
                    // for (size_t v = 0; v < vertexCount; ++v)
                    // {
                    //     Vertex vert {};
                    //     vert.pos    = glm::vec4(glm::make_vec3(&positionBuffer[v * 3]), 1.0f);
                    //     vert.normal = glm::normalize(glm::vec3(normalsBuffer ? glm::make_vec3(&normalsBuffer[v * 3]) : glm::vec3(0.0f)));
                    //     vert.uv     = texCoordsBuffer ? glm::make_vec2(&texCoordsBuffer[v * 2]) : glm::vec3(0.0f);
                    //     vert.color  = glm::vec3(1.0f);
                    // }
                }

                // indices
                {
                    auto accessor   = model.accessors[primitive.indices];
                    auto bufferView = model.bufferViews[accessor.bufferView];

                    if (TINYGLTF_TARGET_ELEMENT_ARRAY_BUFFER == bufferView.target)
                    {
                        auto buffer         = model.buffers[bufferView.buffer];
                        uint32_t indexCount = static_cast<uint32_t>(accessor.count);

                        std::cout << "indices size: " << indexCount << '\n';

                        switch (accessor.componentType)
                        {
                        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
                        {
                            auto buf = &buffer.data[accessor.byteOffset + bufferView.byteOffset];
                            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(uint32_t), buf, GL_STATIC_DRAW);
                            m_vaos.try_emplace(VAO, std::make_tuple(indexCount, GL_UNSIGNED_INT));
                        }
                        break;
                        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
                        {
                            auto buf = &buffer.data[accessor.byteOffset + bufferView.byteOffset];

                            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(uint16_t), buf, GL_STATIC_DRAW);
                            m_vaos.try_emplace(VAO, std::make_tuple(indexCount, GL_UNSIGNED_SHORT));
                        }
                        break;
                        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
                        {
                            auto buf = &buffer.data[accessor.byteOffset + bufferView.byteOffset];
                            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(uint8_t), buf, GL_STATIC_DRAW);
                            m_vaos.try_emplace(VAO, std::make_tuple(indexCount, GL_UNSIGNED_BYTE));
                        }
                        break;
                        default:
                            break;
                        }
                    }
                }

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(0);
                glBindVertexArray(0);
            }
        }
    }

private:
    std::map<uint32_t, std::tuple<int, uint32_t>> m_vaos;
    tinygltf::Model m_model;
};
