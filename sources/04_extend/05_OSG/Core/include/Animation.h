#pragma once

#include "Transform.h"
#include <functional>

class Animation : public Transform
{
public:
    void Accept(Visitor&) override;

    void SetUpdateCallback(std::function<void(glm::mat4&)>&& callback);
    void Update();

private:
    std::function<void(glm::mat4&)> m_updateCallback {};
};
