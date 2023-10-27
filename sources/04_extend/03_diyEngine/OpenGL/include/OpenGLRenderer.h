#pragma once

#include "renderer.h"

class OpenGLRenderer : public Renderer
{
public:
    static OpenGLRenderer* New();

    void Render() override;

protected:
    OpenGLRenderer();
    ~OpenGLRenderer();
};
