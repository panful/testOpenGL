#include "RecordTraversal.h"
#include "Log.h"
#include "View.h"
#include "Geometry.h"
#include "GraphicsPipeline.h"

void RecordTraversal::Apply(Window& window)
{
    Log::GetInstance()->Trace();
}

void RecordTraversal::Apply(View& view)
{
    Log::GetInstance()->Trace();
    view.Clear();
    view.Traverse(this);
}

void RecordTraversal::Apply(Geometry& geometry)
{
    Log::GetInstance()->Trace();
    geometry.Draw();
}

void RecordTraversal::Apply(GraphicsPipeline& graphicsPipeline)
{
    Log::GetInstance()->Trace();
    graphicsPipeline.Bind();
    graphicsPipeline.Traverse(this);
}
