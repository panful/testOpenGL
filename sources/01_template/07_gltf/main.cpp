/**
 * 1. 打印gltf文件的基本信息
 * 2. 打印gltf的顶点、索引、材质等信息
 */

#define TEST2

#ifdef TEST1

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION

#include "tiny_gltf.h"
#include <filesystem>
#include <iostream>

bool loadModel(tinygltf::Model& model, const std::filesystem::path& filename)
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
            res = loader.LoadBinaryFromFile(&model, &err, &warn, filename.string());
        }
        if (filename.extension() == ".gltf")
        {
            res = loader.LoadASCIIFromFile(&model, &err, &warn, filename.string());
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
    }
    else
    {
        std::cout << "Loaded glTF: " << filename << std::endl;
    }

    return res;
}

void dbgModel(tinygltf::Model& model)
{
    for (auto& mesh : model.meshes)
    {
        std::cout << "mesh : " << mesh.name << std::endl;
        for (auto& primitive : mesh.primitives)
        {
            const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];

            std::cout << "indexaccessor: count " << indexAccessor.count << ", type " << indexAccessor.componentType << std::endl;

            if (model.materials.size() > 0 && primitive.material >= 0)
            {
                tinygltf::Material& mat = model.materials[primitive.material];
                for (auto& mats : mat.values)
                {
                    std::cout << "mat : " << mats.first.c_str() << std::endl;
                }
            }

            for (auto& image : model.images)
            {
                std::cout << "image name : " << image.uri << std::endl;
                std::cout << "  size : " << image.image.size() << std::endl;
                std::cout << "  w/h : " << image.width << "/" << image.height << std::endl;
            }

            std::cout << "indices : " << primitive.indices << std::endl;
            std::cout << "mode     : "
                      << "(" << primitive.mode << ")" << std::endl;

            for (auto& attrib : primitive.attributes)
            {
                std::cout << "attribute : " << attrib.first.c_str() << std::endl;
            }
        }
    }
}

int main()
{
    tinygltf::Model model;
    if (loadModel(model, "./resources/Poop.glb"))
    {
        dbgModel(model);
    }

    std::cout << "---------------------------------------------\n";

    if (loadModel(model, "./resources/singleTriangle.gltf"))
    {
        dbgModel(model);
    }
}

#endif // TEST1

#ifdef TEST2

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION

#include "tiny_gltf.h"
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec3 color;
};

struct Texture
{
    int imageIndex { 0 };
};

struct Material
{
    glm::vec4 baseColorFactor { 1.0f };
    int baseColorTextureIndex;
};

struct Image
{
    std::vector<uint8_t> piexels {};
    int width { 0 };
    int height { 0 };
};

void LoadNode(const tinygltf::Node& inputNode, const tinygltf::Model& model)
{
    for (auto child : inputNode.children)
    {
        LoadNode(model.nodes[child], model);
    }

    // vertices indices
    if (inputNode.mesh > -1)
    {
        for (auto& primitive : model.meshes[inputNode.mesh].primitives)
        {
            // vertices
            {
                const float* positionBuffer { nullptr };
                const float* normalsBuffer { nullptr };
                const float* texCoordsBuffer { nullptr };
                size_t vertexCount { 0 };

                if (primitive.attributes.contains("POSITION"))
                {
                    auto accessor   = model.accessors[primitive.attributes.at("POSITION")];
                    auto bufferView = model.bufferViews[accessor.bufferView];
                    positionBuffer
                        = reinterpret_cast<const float*>(&(model.buffers[bufferView.buffer].data[accessor.byteOffset + bufferView.byteOffset]));
                    vertexCount = accessor.count;

                    std::cout << "vertices size: " << vertexCount << '\n';
                }

                if (primitive.attributes.contains("NORMAL"))
                {
                    auto accessor   = model.accessors[primitive.attributes.at("NORMAL")];
                    auto bufferView = model.bufferViews[accessor.bufferView];
                    normalsBuffer
                        = reinterpret_cast<const float*>(&(model.buffers[bufferView.buffer].data[accessor.byteOffset + bufferView.byteOffset]));
                }

                if (primitive.attributes.contains("TEXCOORD_0"))
                {
                    auto accessor   = model.accessors[primitive.attributes.at("TEXCOORD_0")];
                    auto bufferView = model.bufferViews[accessor.bufferView];
                    texCoordsBuffer
                        = reinterpret_cast<const float*>(&(model.buffers[bufferView.buffer].data[accessor.byteOffset + bufferView.byteOffset]));
                }

                // 合并顶点属性
                for (size_t v = 0; v < vertexCount; ++v)
                {
                    Vertex vert {};
                    vert.pos    = glm::vec4(glm::make_vec3(&positionBuffer[v * 3]), 1.0f);
                    vert.normal = glm::normalize(glm::vec3(normalsBuffer ? glm::make_vec3(&normalsBuffer[v * 3]) : glm::vec3(0.0f)));
                    vert.uv     = texCoordsBuffer ? glm::make_vec2(&texCoordsBuffer[v * 2]) : glm::vec3(0.0f);
                    vert.color  = glm::vec3(1.0f);
                }
            }

            // indices
            {
                auto accessor   = model.accessors[primitive.indices];
                auto bufferView = model.bufferViews[accessor.bufferView];

                if (TINYGLTF_TARGET_ELEMENT_ARRAY_BUFFER == bufferView.target)
                {
                    auto buffer          = model.buffers[bufferView.buffer];
                    uint32_t firstIndex  = 0;
                    uint32_t vertexStart = 0;
                    uint32_t indexCount  = static_cast<uint32_t>(accessor.count);

                    std::cout << "indices size: " << indexCount << '\n';

                    std::vector<uint32_t> indexBuffer;
                    switch (accessor.componentType)
                    {
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
                    {
                        const uint32_t* buf = reinterpret_cast<const uint32_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
                        for (size_t index = 0; index < accessor.count; index++)
                        {
                            indexBuffer.emplace_back(buf[index] + vertexStart);
                        }
                    }
                    break;
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
                    {
                        const uint16_t* buf = reinterpret_cast<const uint16_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
                        for (size_t index = 0; index < accessor.count; index++)
                        {
                            indexBuffer.emplace_back(buf[index] + vertexStart);
                        }
                    }
                    break;
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
                    {
                        const uint8_t* buf = reinterpret_cast<const uint8_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
                        for (size_t index = 0; index < accessor.count; index++)
                        {
                            indexBuffer.emplace_back(buf[index] + vertexStart);
                        }
                    }
                    break;
                    default:
                        break;
                    }
                }
            }
        }
    }
}

void LoadTexture(const tinygltf::Model& model)
{
    std::vector<Texture> textures;
    for (auto& texture : model.textures)
    {
        textures.emplace_back(Texture { texture.source });
    }
}

void LoadMaterials(const tinygltf::Model& model)
{
    std::vector<Material> materials;
    for (auto& material : model.materials)
    {
        // Get the base color factor
        if (material.values.contains("baseColorFactor"))
        {
            materials.emplace_back(Material { glm::make_vec4(material.values.at("baseColorFactor").ColorFactor().data()), 0 });
        }

        // Get base color texture index
        if (material.values.contains("baseColorTexture"))
        {
            materials.emplace_back(Material { glm::vec4(1.f), material.values.at("baseColorTexture").TextureIndex() });
        }
    }
}

void LoadImages(const tinygltf::Model& model)
{
    std::vector<Image> images;
    for (auto& image : model.images)
    {
        if (image.component == 3)
        {
            // RGB
            images.emplace_back(Image { image.image, image.width, image.height });
        }
    }
}

bool LoadModel(const std::filesystem::path& filename)
{
    tinygltf::TinyGLTF loader;
    tinygltf::Model model;

    std::string err;
    std::string warn;

    bool res { false };
    if (filename.has_extension())
    {
        auto ex = filename.extension();
        if (filename.extension() == ".glb")
        {
            res = loader.LoadBinaryFromFile(&model, &err, &warn, filename.string());
        }
        if (filename.extension() == ".gltf")
        {
            res = loader.LoadASCIIFromFile(&model, &err, &warn, filename.string());
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
        return false;
    }

    std::cout << "scenes size: " << model.scenes.size() << std::endl;
    std::cout << "nodes size: " << model.nodes.size() << std::endl;

    LoadImages(model);
    LoadTexture(model);
    LoadMaterials(model);

    for (auto& scene : model.scenes)
    {
        for (auto node : scene.nodes)
        {
            const auto glTFNode = model.nodes[static_cast<size_t>(node)];
            LoadNode(glTFNode, model);
        }
    }

    return res;
}

int main()
{
    LoadModel("./resources/Poop.glb");

    std::cout << "---------------------------------------------\n";

    LoadModel("./resources/singleTriangle.gltf");

    std::cout << "---------------------------------------------\n";

    LoadModel("./resources/sponza.gltf");
}

#endif // TEST2