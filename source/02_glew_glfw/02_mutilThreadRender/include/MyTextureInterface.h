#pragma once

class MyTextureInterface
{
public:
    ~MyTextureInterface() = default;
    virtual GLuint getTexture() const = 0;
    virtual void set(GLenum iFormat,  GLuint xsize, GLuint ysize, GLuint zsize = 0,GLenum fmt = 0,GLenum type=0,void* data = nullptr) = 0;
    virtual bool isReady() const = 0;
    virtual void setReady(bool) = 0;
    virtual unsigned int getSize() const = 0;
    virtual GLubyte* getColorData() = 0;
    virtual GLfloat* getDepthData() = 0;
    virtual void genData() = 0;
};