#pragma once

#include "renderer.h"
#include "actor.h"
#include "camera.h"
#include "log.h"
#include "objectFactory.h"
#include "window.h"
#include <algorithm>

ObjectFactoryNewMacro(Renderer);

Renderer::Renderer()
{
    LogDebug("");
    m_camera = Camera::New();
}

Renderer::~Renderer()
{
    LogDebug("");
    DestructObjectMemberMacro(m_camera);
    for (const auto actor : m_actors)
    {
        actor->UnRegister(this);
    }
}

void Renderer::AddActor(Actor* actor)
{
    if (!HasActor(actor))
    {
        m_actors.emplace_back(actor);
        actor->Register(this);
    }
}

void Renderer::RemoveActor(Actor* actor)
{
    if (HasActor(actor))
    {
        m_actors.remove(actor);
        actor->UnRegister(this);
    }
}

bool Renderer::HasActor(Actor* actor)
{
    return !(std::find(std::begin(m_actors), std::end(m_actors), actor) == m_actors.end());
}

void Renderer::Render()
{
    for (const auto actor : m_actors)
    {
        actor->Render();
    }
}

void Renderer::ResetCamera() const
{
}

void Renderer::SetCamera(Camera* camera)
{
    SetObjectBodyMacro(m_camera, camera);
}

Camera* Renderer::GetCamera() const noexcept
{
    return m_camera;
}

void Renderer::SetWindow(Window* window)
{
    m_window = window;
}

/// @brief 设置视口在窗口上的位置
/// @param viewport x0, y0, x1, y1
void Renderer::SetViewport(const std::array<double, 4>& viewport)
{
    m_viewport = viewport;
}

void Renderer::SetBackground(const std::array<double, 3>& color)
{
    m_background      = color;
    m_useGradientBack = false;
}

void Renderer::SetBackground(const std::array<double, 3>& color, const std::array<double, 3>& color2)
{
    m_background      = color;
    m_background2     = color2;
    m_useGradientBack = true;
}