#include <iostream>
#include "FBObject.h"

#include "MyTexture.h"
void FBObject::create(unsigned int xsize, unsigned int ysize, unsigned int zsize)
{

    glCreateFramebuffers(1, &frameBuffer);

    colorTexture = std::make_shared<MyTexture2D>();
    colorTexture->set(GL_RGBA8, xsize, ysize, zsize);
    backColorTexture = std::make_shared<MyTexture2D>();
    backColorTexture->set(GL_RGBA8, xsize, ysize, zsize);

    depthTexture = std::make_shared<MyTexture2D>();
    depthTexture->set(GL_DEPTH_COMPONENT32F, xsize, ysize, zsize);
    backDepthTexture = std::make_shared<MyTexture2D>();
    backDepthTexture->set(GL_DEPTH_COMPONENT32F, xsize, ysize, zsize);
    bind();

}

std::shared_ptr<MyTextureInterface> FBObject::getColorTex() const
{
    return backColorTexture;
}

std::shared_ptr<MyTextureInterface> FBObject::getDepthTex() const
{
    return backDepthTexture;
}

void FBObject::use()
{
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    bind();
}

void FBObject::unUse()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBObject::setReady(bool t)
{
    if (t)
    {
        colorTexture->setReady(t);
        depthTexture->setReady(t);
        std::swap(colorTexture, backColorTexture);
        std::swap(depthTexture, backDepthTexture);
    }
}

bool FBObject::isReady()
{
    return backDepthTexture->isReady() && backColorTexture->isReady();
}


void FBObject::check()
{
    colorTexture->genData();
    depthTexture->genData();
}

void FBObject::bind()
{

    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorTexture->getTexture(), 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture->getTexture(), 0);
    auto res = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (res != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "create framebuffer error!" << std::endl;
    }
    const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);
}

