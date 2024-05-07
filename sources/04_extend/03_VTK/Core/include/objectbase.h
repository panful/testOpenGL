#pragma once

#include <atomic>

class ObjectBase
{
public:
    virtual void Delete();

    static ObjectBase* New()
    {
        return new ObjectBase();
    }

    virtual void Register(ObjectBase* o);

    virtual void UnRegister(ObjectBase* o);

    int GetReferenceCount() const noexcept
    {
        return this->m_refCount;
    }

    void SetReferenceCount(int);

protected:
    ObjectBase();
    virtual ~ObjectBase();

    std::atomic<int32_t> m_refCount;

protected:
    ObjectBase(const ObjectBase&)
    {
    }

    void operator=(const ObjectBase&)
    {
    }
};
