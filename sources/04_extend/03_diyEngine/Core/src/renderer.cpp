#pragma once

#include "renderer.h"
#include "actor.h"
#include "log.h"
#include "objectFactory.h"
#include <algorithm>

ObjectFactoryNewMacro(Renderer);

Renderer::Renderer()
{
    LogDebug("");
}

Renderer::~Renderer()
{
    LogDebug("");
}

void Renderer::AddActor(Actor* actor)
{
    if (!HasActor(actor))
    {
        m_actors.emplace_back(actor);
    }
}

void Renderer::RemoveActor(Actor* actor)
{
    if (HasActor(actor))
    {
        m_actors.remove(actor);
    }
}

bool Renderer::HasActor(Actor* actor)
{
    return !(std::find(std::begin(m_actors), std::end(m_actors), actor) == m_actors.end());
}

void Renderer::Render()
{
    for (const auto& actor : m_actors)
    {
        actor->Render();
    }
}

void Renderer::ResetCamera() const
{
}

void Renderer::SetCamera(Camera* cam)
{
    m_camera = cam;
}

Camera* Renderer::GetCamera() const
{
    return m_camera;
}

void Renderer::SetWindow(Window* w)
{
    m_window = w;
}

/// @brief 设置视口在窗口上的位置
/// @param v x0, y0, x1, y1
void Renderer::SetViewport(const std::array<double, 4>& v)
{
    m_viewport = v;
}

void Renderer::SetBackground(const std::array<double, 3>& c)
{
    m_background      = c;
    m_useGradientBack = false;
}

void Renderer::SetBackground(const std::array<double, 3>& c, const std::array<double, 3>& c2)
{
    m_background      = c;
    m_background2     = c2;
    m_useGradientBack = true;
}