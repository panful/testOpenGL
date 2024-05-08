#pragma once

class Object
{
public:
    Object()                             = default;
    Object(const Object&)                = default;
    Object(Object&&) noexcept            = default;
    Object& operator=(const Object&)     = default;
    Object& operator=(Object&&) noexcept = default;
    virtual ~Object() noexcept           = default;
};
