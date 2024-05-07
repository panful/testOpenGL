#include "OpenGLActor.h"
#include "log.h"
#include "mapper.h"
#include "objectFactory.h"
#include "property.h"

StandardNewMacro(OpenGLActor);

OpenGLActor::OpenGLActor()
{
    LogDebug("");
}

OpenGLActor::~OpenGLActor()
{
    LogDebug("");
}

void OpenGLActor::Render(Renderer* renderer)
{
    m_mapper->Render(renderer, this);
}