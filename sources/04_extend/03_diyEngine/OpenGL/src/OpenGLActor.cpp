#include "OpenGLActor.h"
#include "log.h"
#include "objectFactory.h"
#include "mapper.h"

StandardNewMacro(OpenGLActor);

OpenGLActor::OpenGLActor()
{
    LogDebug("");
}

OpenGLActor::~OpenGLActor()
{
    LogDebug("");
}

void OpenGLActor::Render()
{
    m_mapper->Render();
}