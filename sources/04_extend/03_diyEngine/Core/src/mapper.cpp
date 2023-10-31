#include "mapper.h"
#include "geometry.h"
#include "log.h"
#include "objectFactory.h"

ObjectFactoryNewMacro(Mapper);

Mapper::Mapper()
{
    LogDebug("");
}

Mapper::~Mapper()
{
    LogDebug("");
    DestructObjectMemberMacro(m_geometry);
}

void Mapper::SetGeometry(Geometry* geometry)
{
    SetObjectBodyMacro(m_geometry, geometry);
}

void Mapper::Render(Actor*)
{
}

