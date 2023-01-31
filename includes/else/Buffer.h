#pragma once
#include "glad/glad.h"

#include <iostream>

class Buffer
{
public:
    unsigned int m_FBO, m_RBO, m_texBuffer;
public:
    Buffer();
    ~Buffer();
};

Buffer::Buffer()
    :m_FBO(0),
    m_RBO(0),
    m_texBuffer(0)
{
    glGenFramebuffers(1, &m_FBO);
    glGenRenderbuffers(1, &m_RBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
    //纹理
    glGenTextures(1, &m_texBuffer);
    glBindTexture(GL_TEXTURE_2D, m_texBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    //将纹理附加到当前绑定的帧缓冲对象
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texBuffer, 0);
    //创建一个深度和模板渲染缓冲对象
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
    //附加这个渲染缓冲对象
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO); //将渲染缓冲对象附加到帧缓冲的深度和模板附件上
    //渲染缓冲对象使用结束
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    //检查帧缓冲是否完整
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
}

Buffer::~Buffer()
{

}