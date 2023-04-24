#pragma once


#include <gl/glew.h>
#include <memory>
#include <atomic>

#include "MyTextureInterface.h"

class FBObject
{
public:
    FBObject() :colorTexture(nullptr), depthTexture(nullptr), backColorTexture(nullptr), 
        backDepthTexture(nullptr), frameBuffer(0) {}

    void create(unsigned int xsize, unsigned int ysize, unsigned int zsize = 0);

    std::shared_ptr<MyTextureInterface> getColorTex()const;

    std::shared_ptr<MyTextureInterface> getDepthTex()const;

    void use();

    void unUse();

    /// @brief 当前渲染完成，交换当前纹理对象
    void setReady(bool);

    bool isReady();

    void check();
private:
    void bind();
private:
    std::shared_ptr<MyTextureInterface> colorTexture;
    std::shared_ptr<MyTextureInterface> depthTexture;
    std::shared_ptr<MyTextureInterface> backColorTexture;
    std::shared_ptr<MyTextureInterface> backDepthTexture;
    GLuint frameBuffer;
};
