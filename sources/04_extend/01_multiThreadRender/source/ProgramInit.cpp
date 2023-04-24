#include "ProgramInit.h"
#include "MyShader.h"
void ProgramInitFunc::operator()(std::shared_ptr<MyProgram> base, std::shared_ptr<MyProgram> mixed)
{
    base->install("resources/vertex.glsl", "resources/BaseFrag.glsl");
    mixed->install("resources/vertex.glsl", "resources/MergeFrag.glsl");
}
