#pragma once

#include "object.h"

class Geometry;

class Mapper : public Object
{
public:
    static Mapper* New();

    void SetGeometry(Geometry*);
    virtual void Render();

protected:
    Mapper();
    ~Mapper() override;

protected:
    Geometry* m_geometry { nullptr };
    bool m_dirty { true };
};