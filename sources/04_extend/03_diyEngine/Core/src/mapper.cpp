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
}

void Mapper::SetGeometry(Geometry* geom)
{
    m_geometry = geom;
}

void Mapper::Render()
{
}

