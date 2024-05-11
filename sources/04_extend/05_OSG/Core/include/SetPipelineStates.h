#pragma once

#include "Visitor.h"

class SetPipelineStates : public Visitor
{
public:
    void Apply(State& state) override;
    void Apply(RasterizationState& rasterizationState) override;
    void Apply(InputAssemblyState& inputAssemblyState) override;
    void Apply(ColorBlendState& colorBlendState) override;
    void Apply(DepthStencilState& depthStencilState) override;
};
