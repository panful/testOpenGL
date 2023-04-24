#pragma once

#include <gl/glew.h>
#include <atomic>
#include "MyTextureInterface.h"
/// <summary>
/// @details 仅有一层的二维纹理
/// </summary>
class MyTexture2D:public MyTextureInterface 
{
public:
    MyTexture2D();

    ~MyTexture2D();

    void destroy();

    void setSize(GLuint xsize, GLuint ysize, GLuint zsize=0);

    GLuint getXSize()
    {
        return xsize;
    }
    GLuint getYSize()
    {
        return ysize;
    }
    GLuint getZSize()
    {
        return zsize;
    }

    void setInternalFormat(GLenum iFormat)
    {
        internalFormat = iFormat;
    }

    void setFormat(GLenum fmt)
    {
        format = fmt;
    }
    
    void setDataType(GLenum tp)
    {
        type = tp;
    }
    virtual void set(GLenum iFormat, GLuint xsize, GLuint ysize, GLuint zsize = 0, GLenum fmt = 0, GLenum type = 0, void* data = nullptr) override;
    
    virtual bool isReady() const override;

    virtual void setReady(bool )override;

    virtual GLuint getTexture()const override;

    virtual unsigned int getSize() const override;
    
    virtual void genData() override;

    virtual GLubyte* getColorData() override;

    virtual GLfloat* getDepthData() override;
private:
    void create(void *data=nullptr);
private:
    GLenum internalFormat{};
    GLenum format{};
    GLenum type{};
    GLuint xsize{};
    GLuint ysize{};
    GLuint zsize{};
    GLuint texture{};
    std::atomic_bool ready{false};
    
    GLubyte* colorData{};
    GLfloat* depthData{};
};