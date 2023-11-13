
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
