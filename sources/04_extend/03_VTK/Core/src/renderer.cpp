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
        actor->Render(this);
    }
}

void Renderer::RenderOpaque()
{
    for (const auto actor : m_actors)
    {
        actor->Render(this);
    }
}

void Renderer::RenderTranslucent()
{
}

void Renderer::Clear()
{
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
    // 不能增加window的引用计数，避免循环依赖
    m_window = window;
}

Window* Renderer::GetWindow() const noexcept
{
    return m_window;
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