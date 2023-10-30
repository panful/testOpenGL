#pragma once

#include "object.h"

class Camera : public Object
{
public:
    static Camera* New();

protected:
    Camera();
    ~Camera() override;
};