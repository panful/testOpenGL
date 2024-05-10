#include "Animation.h"
#include "Log.h"
#include "Visitor.h"

void Animation::Accept(Visitor& visitor)
{
    visitor.Apply(*this);
}

void Animation::SetUpdateCallback(std::function<void(glm::mat4&)>&& callback)
{
    m_updateCallback = std::move(callback);
}

void Animation::Update()
{
    Log::GetInstance()->Trace();
    m_updateCallback(m_matrix);
}
