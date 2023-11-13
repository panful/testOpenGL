
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
#include <iostream>

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
                if (primitive.attributes.contains("POSITION"))
                {
                    auto accessor   = model.accessors[primitive.attributes.at("POSITION")];
                    auto bufferView = model.bufferViews[accessor.bufferView];
                    auto positionBuffer
                        = reinterpret_cast<const float*>(&(model.buffers[bufferView.buffer].data[accessor.byteOffset + bufferView.byteOffset]));
                    auto vertexCount = accessor.count;

                    std::cout << "vertices size: " << vertexCount << '\n';
                }

                if (primitive.attributes.contains("NORMAL"))
                {
                    auto accessor   = model.accessors[primitive.attributes.at("NORMAL")];
                    auto bufferView = model.bufferViews[accessor.bufferView];
                    auto normalBuffer
                        = reinterpret_cast<const float*>(&(model.buffers[bufferView.buffer].data[accessor.byteOffset + bufferView.byteOffset]));
                }

                if (primitive.attributes.contains("TEXCOORD_0"))
                {
                    auto accessor   = model.accessors[primitive.attributes.at("TEXCOORD_0")];
                    auto bufferView = model.bufferViews[accessor.bufferView];
                    auto texCoordBuffer
                        = reinterpret_cast<const float*>(&(model.buffers[bufferView.buffer].data[accessor.byteOffset + bufferView.byteOffset]));
                }
            }

            // indices
            {
                auto accessor   = model.accessors[primitive.indices];
                auto bufferView = model.bufferViews[accessor.bufferView];
                if (TINYGLTF_TARGET_ELEMENT_ARRAY_BUFFER == bufferView.target)
                {
                    auto buffer     = model.buffers[bufferView.buffer];
                    auto indexCount = static_cast<uint32_t>(accessor.count);

                    std::cout << "indices size: " << indexCount << '\n';

                    switch (accessor.componentType)
                    {
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
                        // uint32_t
                        break;
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
                        // uint16_t
                        break;
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
                        // uint8_t
                        break;
                    default:
                        break;
                    }
                }
            }
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
}

#endif // TEST2