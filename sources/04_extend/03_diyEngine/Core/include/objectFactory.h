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
