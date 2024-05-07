#pragma once

#include "object.h"
#include <functional>
#include <map>
#include <string>

class ObjectFactory
{
protected:
    struct OverrideInfo
    {
        std::function<Object*()> CreateFunction;
    };

public:
    static ObjectFactory* GetInstance();
    Object* CreateOverride(const std::string& className);

protected:
    ObjectFactory();
    virtual ~ObjectFactory();
    void RegisterOverride(const std::string&, OverrideInfo&&);

    std::map<std::string, OverrideInfo> m_overrideInformation;
};

#define CreateObjectFunction(className)                  \
    static Object* TempCreateObjectFunction##className() \
    {                                                    \
        return className::New();                         \
    }

#define StandardNewMacro(thisClass) \
    thisClass* thisClass::New()     \
    {                               \
        return new thisClass();     \
    }

#define ObjectFactoryNewMacro(thisClass)                                         \
    thisClass* thisClass::New()                                                  \
    {                                                                            \
        if (auto ret = ObjectFactory::GetInstance()->CreateOverride(#thisClass)) \
        {                                                                        \
            return static_cast<thisClass*>(ret);                                 \
        }                                                                        \
        return thisClass::New();                                                 \
    }

#define SetObjectBodyMacro(Name, Obj) \
    if (Name != Obj)                  \
    {                                 \
        auto tmp = Name;              \
        Name     = Obj;               \
        if (Name)                     \
        {                             \
            Name->Register(this);     \
        }                             \
        if (tmp)                      \
        {                             \
            tmp->UnRegister(this);    \
        }                             \
    }

#define ConstructObjectMemberMacro(Name, Type) Name = Type::New();

#define DestructObjectMemberMacro(Name) \
    if (Name)                           \
    {                                   \
        Name->Delete();                 \
        Name = nullptr;                 \
    }