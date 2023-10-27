#include "objectFactory.h"
#include "openGLObjectFactory.h"

ObjectFactory* ObjectFactory::GetInstance()
{
    static OpenGLObjectFactory factory;
    // static VulkanObjectFactory factory;

    return &factory;
}

ObjectFactory::ObjectFactory()
{
}

ObjectFactory::~ObjectFactory()
{
}

Object* ObjectFactory::CreateOverride(const std::string& className)
{
    if (m_overrideInformation.count(className))
    {
        return m_overrideInformation.at(className).CreateFunction();
    }
    return nullptr;
}

void ObjectFactory::RegisterOverride(const std::string& name, OverrideInfo&& info)
{
    m_overrideInformation[name] = info;
}