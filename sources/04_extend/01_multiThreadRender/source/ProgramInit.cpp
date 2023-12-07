#include "ProgramInit.h"
#include "MyShader.h"
void ProgramInitFunc::operator()(std::shared_ptr<MyProgram> base, std::shared_ptr<MyProgram> mixed)
{
    base->install("shaders/vertex.glsl", "shaders/BaseFrag.glsl");
    mixed->install("shaders/vertex.glsl", "shaders/MergeFrag.glsl");
}
