#include "OpenGLMapper.h"
#include "OpenGLIndexBufferObject.h"
#include "OpenGLShaderProgram.h"
#include "OpenGLVertexArrayObject.h"
#include "OpenGLVertexBufferObject.h"
#include "cells.h"
#include "geometry.h"
#include "log.h"
#include "objectFactory.h"
#include "points.h"
#include "smartpointer.h"
#include <glad/glad.h>

StandardNewMacro(OpenGLMapper);

OpenGLMapper::OpenGLMapper()
{
    LogDebug("");
}

OpenGLMapper::~OpenGLMapper()
{
    LogDebug("");
}

void OpenGLMapper::Render()
{
    if (m_dirty)
    {
        BuildBufferObjects();
        BuildShaderProgram();

        m_dirty = false;
    }

    m_shaderProgram->Use();
    m_vao->Bind();

    if (m_iboVertex)
    {
        m_iboVertex->Bind();
        glDrawElements(GL_POINTS, static_cast<GLsizei>(m_geometry->GetCells()->GetVertexIndices().size()), GL_UNSIGNED_INT, 0);
    }
    if (m_iboLine)
    {
        m_iboLine->Bind();
        glDrawElements(GL_LINES, static_cast<GLsizei>(m_geometry->GetCells()->GetLineIndices().size()), GL_UNSIGNED_INT, 0);
    }
    if (m_iboTriangle)
    {
        m_iboTriangle->Bind();
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_geometry->GetCells()->GetTriangleIndices().size()), GL_UNSIGNED_INT, 0);
    }

    m_vao->UnBind();
}

void OpenGLMapper::BuildBufferObjects()
{
    m_vao = SmartPointer<OpenGLVertexArrayObject>::New();
    m_vbo = SmartPointer<OpenGLVertexBufferObject>::New();

    m_vao->Bind();
    m_vbo->Bind();
    m_vbo->UpLoad(m_geometry->GetPoints()->GetPoints());
    m_vao->VertexAttrib();
    m_vao->UnBind();

    if (!m_geometry->GetCells()->GetVertexIndices().empty())
    {
        m_iboVertex = SmartPointer<OpenGLIndexBufferObject>::New();
        m_iboVertex->Bind();
        m_iboVertex->UpLoad(m_geometry->GetCells()->GetVertexIndices());
        m_iboVertex->UnBind();
    }
    if (!m_geometry->GetCells()->GetLineIndices().empty())
    {
        m_iboLine = SmartPointer<OpenGLIndexBufferObject>::New();
        m_iboLine->Bind();
        m_iboLine->UpLoad(m_geometry->GetCells()->GetLineIndices());
        m_iboLine->UnBind();
    }
    if (!m_geometry->GetCells()->GetTriangleIndices().empty())
    {
        m_iboTriangle = SmartPointer<OpenGLIndexBufferObject>::New();
        m_iboTriangle->Bind();
        m_iboTriangle->UpLoad(m_geometry->GetCells()->GetTriangleIndices());
        m_iboTriangle->UnBind();
    }
}

void OpenGLMapper::BuildShaderProgram()
{
    m_shaderProgram = SmartPointer<OpenGLShaderProgram>::New();
    m_shaderProgram->SetAttribute();
}