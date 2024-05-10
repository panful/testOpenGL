#pragma once

#include "Visitor.h"
#include <cstdint>
#include <glm/mat4x4.hpp>

class RecordTraversal : public Visitor
{
public:
    void Apply(Window&) override;
    void Apply(View&) override;
    void Apply(Geometry&) override;
    void Apply(GraphicsPipeline&) override;
    void Apply(Transform&) override;
    void Apply(Animation&) override;

private:
    uint32_t m_shaderProgramHandle { 0 };
    glm::mat4 m_matrix { glm::mat4(1.f) };
    View* m_view {};
};
