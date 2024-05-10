#include "RecordTraversal.h"
#include "Animation.h"
#include "Camera.h"
#include "Geometry.h"
#include "GraphicsPipeline.h"
#include "Log.h"
#include "Transform.h"
#include "View.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

void RecordTraversal::Apply(Window& window)
{
    Log::GetInstance()->Trace();
}

void RecordTraversal::Apply(View& view)
{
    Log::GetInstance()->Trace();
    m_view = &view;
    view.Clear();
    view.Traverse(this);
}

void RecordTraversal::Apply(Geometry& geometry)
{
    Log::GetInstance()->Trace();

    if (auto location = glGetUniformLocation(m_shaderProgramHandle, "model"); location != -1)
    {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m_matrix));
    }

    geometry.Draw();
}

void RecordTraversal::Apply(GraphicsPipeline& graphicsPipeline)
{
    Log::GetInstance()->Trace();
    m_shaderProgramHandle = graphicsPipeline.GetHandle();
    graphicsPipeline.Bind();

    glm::mat4 view_proj = m_view->GetCamera()->GetProjectMatrix() * m_view->GetCamera()->GetViewMatrix();

    if (auto location = glGetUniformLocation(m_shaderProgramHandle, "default_view_proj"); location != -1)
    {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(view_proj));
    }

    graphicsPipeline.Traverse(this);
}

void RecordTraversal::Apply(Transform& transform)
{
    Log::GetInstance()->Trace();
    m_matrix *= transform.GetMatrix();
    transform.Traverse(this);
}

void RecordTraversal::Apply(Animation& animation)
{
    Log::GetInstance()->Trace();
    animation.Update();
    m_matrix *= animation.GetMatrix();
    animation.Traverse(this);
}
