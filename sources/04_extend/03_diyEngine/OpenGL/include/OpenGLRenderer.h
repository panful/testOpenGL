#pragma once

#include "renderer.h"

class OpenGLRenderer : public Renderer
{
public:
    static OpenGLRenderer* New();

    void Render() override;
    void RenderOpaque() override;
    void RenderTranslucent() override;
    void Clear() override;

protected:
    OpenGLRenderer();
    ~OpenGLRenderer() override;
};
