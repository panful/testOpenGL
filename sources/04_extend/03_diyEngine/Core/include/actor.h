#pragma once

#include "object.h"

class Mapper;
class Property;

class Actor : public Object
{
public:
    static Actor* New();

    void SetMapper(Mapper*);
    virtual void Render();

    void SetProperty(Property*);
    Property* GetProperty() const noexcept;

protected:
    Actor();
    ~Actor() override;

protected:
    Mapper* m_mapper { nullptr };
    Property* m_property { nullptr };
};