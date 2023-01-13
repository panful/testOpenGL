#pragma once
#include <memory>

class MyProgram;

class ProgramInitFunc
{
public:
    void operator()(std::shared_ptr<MyProgram> base, std::shared_ptr<MyProgram> mixed);
};