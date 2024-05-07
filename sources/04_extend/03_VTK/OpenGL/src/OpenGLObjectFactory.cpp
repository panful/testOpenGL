#include "OpenGLObjectFactory.h"
#include "OpenGLActor.h"
#include "OpenGLMapper.h"
#include "OpenGLRenderer.h"
#include "log.h"

CreateObjectFunction(OpenGLActor);
CreateObjectFunction(OpenGLMapper);
CreateObjectFunction(OpenGLRenderer);

OpenGLObjectFactory::OpenGLObjectFactory()
{
    LogDebug("");

    RegisterOverride("Actor", { TempCreateObjectFunctionOpenGLActor });
    RegisterOverride("Mapper", { TempCreateObjectFunctionOpenGLMapper });
    RegisterOverride("Renderer", { TempCreateObjectFunctionOpenGLRenderer });
}

OpenGLObjectFactory::~OpenGLObjectFactory()
{
    LogDebug("");
}
