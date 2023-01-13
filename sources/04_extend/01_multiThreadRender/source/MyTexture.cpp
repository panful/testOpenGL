#include <iostream>
#include "MyTexture.h"

MyTexture2D::MyTexture2D()
{
    
}

MyTexture2D::~MyTexture2D()
{
    destroy();
}

void MyTexture2D::create(void* data)
{

    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexStorage2D(GL_TEXTURE_2D, 1, internalFormat, xsize, ysize);
    auto res = glGetError();
    if (data)
    {
        glTextureSubImage2D(texture, 0, 0, 0, xsize, ysize, format, type, data);
    }

}

void MyTexture2D::destroy()
{
    if (texture)
    {
        glDeleteTextures(1, &texture);
    }
    texture = 0;
}

void MyTexture2D::setSize(GLuint xsize, GLuint ysize, GLuint zsize)
{
    destroy();
    this->xsize = xsize;
    this->ysize = ysize;
    this->zsize = zsize;

}

void MyTexture2D::set(GLenum iFormat,  GLuint xsize, GLuint ysize,  GLuint zsize, GLenum fmt, GLenum type, void* data)
{
    setSize(xsize, ysize, zsize);
    setInternalFormat(iFormat);
    setFormat(fmt);
    setDataType(type);
    create(data);
    
}

bool MyTexture2D::isReady() const
{
    return ready;
}

void MyTexture2D::setReady(bool t)
{
    ready = t;
}

GLuint MyTexture2D::getTexture() const
{
    return texture;
}

unsigned int MyTexture2D::getSize() const
{
    return xsize * ysize;
}

void MyTexture2D::genData()
{
    if (internalFormat == GL_RGBA8)
    {
        if (!colorData)
        {
            colorData = new GLubyte[getSize() * 4]{};
        }
        auto cltex = texture;
        glGetTextureImage(cltex, 0, GL_RGBA, GL_UNSIGNED_BYTE, getSize() * 4, colorData);
    }
    else if (internalFormat == GL_DEPTH_COMPONENT32F)
    {
        if (!depthData)
        {
            depthData = new GLfloat[getSize() * 4]{};
        }
        auto cltex = texture;
        glGetTextureImage(cltex, 0, GL_DEPTH_COMPONENT, GL_FLOAT, getSize() * 4, depthData);
    }
}

GLubyte* MyTexture2D::getColorData()
{
    return colorData;
}

GLfloat* MyTexture2D::getDepthData()
{
    return depthData;
}
