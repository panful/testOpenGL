#pragma once

#include "object.h"

class Mapper;
class Property;
class Renderer;

class Actor : public Object
{
public:
    static Actor* New();

    void SetMapper(Mapper*);
    virtual void Render(Renderer*);

    bool IsTranslucent() const noexcept;

    void SetProperty(Property*);
    Property* GetProperty() const noexcept;

protected:
    Actor();
    ~Actor() override;

protected:
    Mapper* m_mapper { nullptr };
    Property* m_property { nullptr };
};