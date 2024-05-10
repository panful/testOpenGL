#pragma once

#include "Node.h"

class Event : public Node
{
public:
    void Accept(Visitor&) override;
};

//-------------------------------------------------------
class KeyEvent : public Event
{
public:
    void Accept(Visitor&) override;
};

class KeyPressEvent : public KeyEvent
{
public:
    void Accept(Visitor&) override;
};

class KeyReleaseEvent : public KeyEvent
{
public:
    void Accept(Visitor&) override;
};

//-------------------------------------------------------
class MouseEvent : public Event
{
public:
    void Accept(Visitor&) override;
};

class MousePressEvent : public MouseEvent
{
public:
    void Accept(Visitor&) override;
};

class MouseReleaseEvent : public MouseEvent
{
public:
    void Accept(Visitor&) override;
};

class MouseMoveEvent : public MouseEvent
{
public:
    void Accept(Visitor&) override;
};
