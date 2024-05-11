#include "SetPipelineStates.h"
#include "State.h"

#include <glad/glad.h>

void SetPipelineStates::Apply(State& state)
{
}

void SetPipelineStates::Apply(RasterizationState& rasterizationState)
{
}

void SetPipelineStates::Apply(InputAssemblyState& inputAssemblyState)
{
    glPolygonMode(inputAssemblyState.m_face, inputAssemblyState.m_mode);
}

void SetPipelineStates::Apply(ColorBlendState& colorBlendState)
{
}

void SetPipelineStates::Apply(DepthStencilState& depthStencilState)
{
}
