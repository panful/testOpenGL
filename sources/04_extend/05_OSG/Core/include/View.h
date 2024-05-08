#pragma once

#include "Group.h"

class View : public Group
{
public:
    void Accept(Visitor& visitor) override;

    void Clear() const noexcept;
};
