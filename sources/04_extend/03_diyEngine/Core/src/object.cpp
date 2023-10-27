#include "object.h"
#include "log.h"

Object* Object::New()
{
    return new Object;
}

Object::Object()
{
}

Object::~Object()
{
    if (this->m_refCount > 0)
    {
        LogError("reference count greater than 0")
    }
}
