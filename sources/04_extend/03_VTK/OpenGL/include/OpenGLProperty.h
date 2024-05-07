#pragma once

#include "property.h"

class OpenGLProperty : public Property
{
public:
    static OpenGLProperty* New();

    void Render() override;

protected:
    OpenGLProperty();
    ~OpenGLProperty() override;
};
