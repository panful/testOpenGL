#include "CompileTraversal.h"
#include "Geometry.h"
#include "GraphicsPipeline.h"
#include "Log.h"
#include "View.h"
#include "Transform.h"
#include "Animation.h"

void CompileTraversal::Apply(Window& window)
{
    Log::GetInstance()->Trace();
}

void CompileTraversal::Apply(View& view)
{
    Log::GetInstance()->Trace();
    view.Traverse(this);
}

void CompileTraversal::Apply(Geometry& geometry)
{
    Log::GetInstance()->Trace();
    geometry.Compile();
}

void CompileTraversal::Apply(GraphicsPipeline& graphicsPipeline)
{
    Log::GetInstance()->Trace();
    graphicsPipeline.Compile();
    graphicsPipeline.Traverse(this);
}

void CompileTraversal::Apply(Transform& transform)
{
    Log::GetInstance()->Trace();
    transform.Traverse(this);
}

void CompileTraversal::Apply(Animation& animation)
{
    Log::GetInstance()->Trace();
    animation.Traverse(this);
}
