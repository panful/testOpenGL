#pragma once

#include <memory>
#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <iostream>
#include "MyTextureInterface.h"

class PrintErrorLogInterface
{
public:
    virtual ~PrintErrorLogInterface() = default;
protected:
    enum class State
    {
        FAILED,
        SUCCESS,
    };
protected:

    virtual State getErrorInfo(GLuint id, GLenum state);
    
};

class MyProgram :public PrintErrorLogInterface
{

    class MyShader :public PrintErrorLogInterface
    {

    public:
        MyShader() :vertexShader(0), fragShader(0)
        {

        }

        ~MyShader();

        bool install(const std::string& vsFileName, const std::string& fsFileName);

        void uninstall();

        GLuint createShader(const std::string& FileName, GLenum shaderType);

        void deleteShader(GLuint& shader);

        GLuint getVS() const
        {
            return vertexShader;
        }
        GLuint getFS() const
        {
            return fragShader;
        }
    private:
        GLuint vertexShader;
        GLuint fragShader;

    };

public:
    MyProgram();
    ~MyProgram() = default;

    void install(const std::string& vsFileName, const std::string& fsFileName);

    void uninstall();

    void setMatrix(const std::string& label, const glm::mat4& mat);

    void setFloat(const std::string& label, float val);

    void setTexture(int bingdingId, std::shared_ptr<MyTextureInterface> texture);

    void use();
    
    void unUse();
    
    
private:
    GLuint createProgram();

    void deleteProgram(GLuint& pro);

private:


    std::shared_ptr<MyShader> shader;
    GLuint program;
};