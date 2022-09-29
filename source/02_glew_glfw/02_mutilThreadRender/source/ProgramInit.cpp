#include "ProgramInit.h"
#include "MyShader.h"
void ProgramInitFunc::operator()(std::shared_ptr<MyProgram> base, std::shared_ptr<MyProgram> mixed)
{
    base->install("vertex.glsl", "BaseFrag.glsl");
    mixed->install("vertex.glsl", "MergeFrag.glsl");
}
