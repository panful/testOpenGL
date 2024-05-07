#pragma once

#include "objectFactory.h"

class OpenGLObjectFactory : public ObjectFactory
{
public:
    OpenGLObjectFactory();
    ~OpenGLObjectFactory() override;
};