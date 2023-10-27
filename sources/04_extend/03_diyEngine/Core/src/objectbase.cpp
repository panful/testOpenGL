
#include "objectbase.h"
#include "log.h"

ObjectBase::ObjectBase()
{
    this->m_refCount = 1;
}

ObjectBase::~ObjectBase()
{
    if (this->m_refCount > 0)
    {
        LogError("reference count greater than 0")
    }
}

void ObjectBase::Delete()
{
    this->UnRegister(static_cast<ObjectBase*>(nullptr));
}

void ObjectBase::SetReferenceCount(int ref)
{
    this->m_refCount = ref;
}

void ObjectBase::Register(ObjectBase* o)
{
    this->m_refCount++;
}

void ObjectBase::UnRegister(ObjectBase* o)
{
    if (--this->m_refCount <= 0)
    {
        delete this;
    }
}
