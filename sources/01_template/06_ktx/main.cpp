
/*
 * 下载release安装包 https://github.com/KhronosGroup/KTX-Software/releases
 * 安装的时候勾选 Development 设置环境变量
 */

#include <iostream>
#include <ktx.h>

int main()
{
    ktxTexture* ktxTexture { nullptr };

    if (ktxResult result
        = ktxTexture_CreateFromNamedFile("../shaders/textures/metalplate01_rgba.ktx", KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktxTexture))
    {
        std::cout << "failed to load texture\n";
        return -1;
    }

    auto width     = ktxTexture->baseWidth;
    auto height    = ktxTexture->baseHeight;
    auto mipLevels = ktxTexture->numLevels;

    ktx_uint8_t* ktxTextureData = ktxTexture_GetData(ktxTexture);

    std::cout << "load texture metalplate01_rgba.ktx successful" << std::endl
              << "width:\t" << width << "\nheight:\t" << height << "\nmips:\t" << mipLevels << std::endl;
}