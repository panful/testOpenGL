#pragma once

#include "object.h"

class LookupTable : public Object
{
public:
    static LookupTable* New();

protected:
    LookupTable();
    ~LookupTable() override;
};