#include "actor.h"
#include "OpenGLActor.h"
#include "log.h"
#include "mapper.h"
#include "objectFactory.h"
#include "property.h"
#include "smartpointer.h"

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

void Actor::Render()
{
    m_mapper->Render();
}

void Actor::SetMapper(Mapper* mapper)
{
    SetObjectBodyMacro(m_mapper, mapper);
}

void Actor::SetProperty(Property* property)
{
    SetObjectBodyMacro(m_property, property);
}

Property* Actor::GetProperty() const noexcept
{
    return m_property;
}