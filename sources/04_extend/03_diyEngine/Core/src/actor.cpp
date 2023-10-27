#include "actor.h"
#include "log.h"
#include "mapper.h"
#include "objectFactory.h"
#include "OpenGLActor.h"

ObjectFactoryNewMacro(Actor);

Actor::Actor()
{
    LogDebug("");
}

Actor::~Actor()
{
    LogDebug("");
}

void Actor::Render()
{
    m_mapper->Render();
}

void Actor::SetMapper(Mapper* mapper)
{
    m_mapper = mapper;
}
