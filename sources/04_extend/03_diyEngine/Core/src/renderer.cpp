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