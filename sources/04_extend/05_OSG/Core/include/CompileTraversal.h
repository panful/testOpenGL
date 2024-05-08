#pragma once

#include "Visitor.h"

class CompileTraversal : public Visitor
{
public:
    void Apply(Window&) override;
    void Apply(View&) override;
    void Apply(Geometry&) override;
    void Apply(GraphicsPipeline&) override;
};
