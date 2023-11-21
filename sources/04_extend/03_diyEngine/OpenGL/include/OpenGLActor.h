#pragma once

#include "actor.h"

class OpenGLActor : public Actor
{
public:
    static OpenGLActor* New();

    void Render(Renderer*) override;

protected:
    OpenGLActor();
    ~OpenGLActor() override;
};
