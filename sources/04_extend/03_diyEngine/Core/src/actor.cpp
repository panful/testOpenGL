#include "actor.h"
#include "OpenGLActor.h"
#include "log.h"
#include "mapper.h"
#include "objectFactory.h"
#include "property.h"
#include "smartpointer.h"
#include <limits>

ObjectFactoryNewMacro(Actor);

Actor::Actor()
{
    LogDebug("");
    m_property = Property::New();
}

Actor::~Actor()
{
    LogDebug("");
    DestructObjectMemberMacro(m_property);
    DestructObjectMemberMacro(m_mapper);
}

void Actor::Render(Renderer* renderer)
{
    if(m_property)
    {
        m_property->Render();
    }
    m_mapper->Render(renderer,this);
}

void Actor::SetMapper(Mapper* mapper)
{
    SetObjectBodyMacro(m_mapper, mapper);
}

bool Actor::IsTranslucent() const noexcept
{
    if (m_property && m_property->GetOpacity() < 1.0)
    {
        return true;
    }
    return false;
}

void Actor::SetProperty(Property* property)
{
    SetObjectBodyMacro(m_property, property);
}

Property* Actor::GetProperty() const noexcept
{
    return m_property;
}