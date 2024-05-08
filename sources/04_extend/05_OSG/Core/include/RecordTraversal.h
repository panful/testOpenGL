#pragma once

#include "Visitor.h"

class RecordTraversal : public Visitor
{
public:
    void Apply(Window&) override;
    void Apply(View&) override;
    void Apply(Geometry&) override;
    void Apply(GraphicsPipeline&) override;
};
