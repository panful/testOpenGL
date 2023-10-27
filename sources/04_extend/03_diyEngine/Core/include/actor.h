#pragma once

#include "object.h"

class Mapper;

class Actor : public Object
{
public:
    static Actor* New();

    void SetMapper(Mapper*);
    virtual void Render();

protected:
    Actor();
    ~Actor() override;

protected:
    Mapper* m_mapper { nullptr };
};