#include "MyShader.h"
#include <fstream>
#include <sstream>

MyProgram::MyShader::~MyShader()
{
    uninstall();
}

bool MyProgram::MyShader::install(const std::string& vsFileName, const std::string& fsFileName)
{
    vertexShader = createShader(vsFileName, GL_VERTEX_SHADER);
    if (!vertexShader)
    {
        return false;
    }
    else
    {
        fragShader = createShader(fsFileName, GL_FRAGMENT_SHADER);
        if (!fragShader)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    return true;
}

void MyProgram::MyShader::uninstall()
{
    deleteShader(vertexShader);
    deleteShader(fragShader);
}

GLuint MyProgram::MyShader::createShader(const std::string& FileName, GLenum shaderType)
{
    std::ifstream infile(FileName);
    if (!infile)
    {
        std::cout << "shader file is error\n";
        return 0;
    }
    GLuint shader{};
    auto buffer = infile.rdbuf();
    std::stringstream stream;
    stream << buffer;
    auto str = stream.str();
    auto shadercode = str.c_str();
    const GLint len = str.size();
    shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shadercode, &len);
    glCompileShader(shader);
    if (getErrorInfo(shader, GL_COMPILE_STATUS) == State::SUCCESS)
    {
        return shader;
    }
    else if (getErrorInfo(shader, GL_COMPILE_STATUS) == State::FAILED)
    {
        return 0;
    }
    return 0;
}

void MyProgram::MyShader::deleteShader(GLuint & shader)
{
    if (shader)
    {
        glDeleteShader(shader);
        shader = 0;
    }
}



MyProgram::MyProgram()
{
    shader = std::make_shared<MyShader>();

}

void MyProgram::install(const std::string& vsFileName, const std::string& fsFileName)
{
    shader->install(vsFileName, fsFileName);
    program = createProgram();
}

void MyProgram::uninstall()
{
    glDetachShader(program, shader->getVS());
    glDetachShader(program, shader->getFS());
    shader->uninstall();
    deleteProgram(program);
}

void MyProgram::setMatrix(const std::string& label, const glm::mat4& mat)
{
    auto loc = glGetUniformLocation(program, label.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE,glm::value_ptr(mat));
}

void MyProgram::setFloat(const std::string& label, float val)
{
    auto loc = glGetUniformLocation(program, label.c_str());
    glUniform1f(loc,val);
}

void MyProgram::setTexture(int bingdingId, std::shared_ptr<MyTextureInterface> texture)
{
    glBindTextureUnit(bingdingId, texture->getTexture());

    
}

void MyProgram::use()
{
    glUseProgram(program);
}

void MyProgram::unUse()
{
    glUseProgram(0);
}

GLuint MyProgram::createProgram()
{
    GLuint ret = 0;
    GLuint pro{};
    pro = glCreateProgram();
    glAttachShader(pro, shader->getVS());
    glAttachShader(pro, shader->getFS());
    glLinkProgram(pro);
    if (getErrorInfo(pro, GL_LINK_STATUS) == State::SUCCESS)
    {
        ret = pro;
    }
    else
    {
        ret = 0;
    }
    return ret;
}

void MyProgram::deleteProgram(GLuint & pro)
{
    glDeleteProgram(pro);
    pro = 0;
}



PrintErrorLogInterface::State PrintErrorLogInterface::getErrorInfo(GLuint id, GLenum state)
{
    GLint shaderstate;
    if (dynamic_cast<MyProgram*>(this))
    {
        glGetProgramiv(id, state, &shaderstate);
    }
    else
    {
        glGetShaderiv(id, state, &shaderstate);
    }
    if (shaderstate == GL_TRUE)
    {
        return State::SUCCESS;
    }
    else
    {
        std::string str;
        int loglen = 0;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &loglen);
        str.resize(loglen);
        glGetShaderInfoLog(id, loglen, NULL, const_cast<GLchar*>(str.c_str()));
        std::cout << str << std::endl;
        return State::FAILED;
    }
}
