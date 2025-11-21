/**
 * 1. 使用 happly 读取 ply 文件
 *
 */

#define TEST1

#ifdef TEST1

#include "happly/happly.h"
#include <format>
#include <iostream>

int main()
{
    std::string file_name { "./models/cube.ply" };
    try
    {
        happly::PLYData plyIn(file_name);

        for (auto& name : plyIn.getElementNames())
        {
            happly::Element& element = plyIn.getElement(name);
            std::cout << std::format("Element name: {}, count: {}\n", element.name, element.count);

            for (auto& property : element.properties)
            {
                std::cout << std::format("  Property name: {}, size: {}\n", property->name, property->size());
            }
        }
    }
    catch (std::exception ex)
    {
        std::cout << std::format("Failed to read ply file: {}\nexception: {}\n", file_name, ex.what());
    }
}

#endif // TEST1
