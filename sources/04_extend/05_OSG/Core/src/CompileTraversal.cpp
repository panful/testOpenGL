#include "CompileTraversal.h"
#include "Geometry.h"
#include "GraphicsPipeline.h"
#include "Log.h"
#include "View.h"

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
