#include "OpenGLMapper.h"
#include "OpenGLIndexBufferObject.h"
#include "OpenGLShaderProgram.h"
#include "OpenGLVertexArrayObject.h"
#include "OpenGLVertexBufferObject.h"
#include "actor.h"
#include "cells.h"
#include "geometry.h"
#include "log.h"
#include "objectFactory.h"
#include "points.h"
#include "property.h"
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
    DestructObjectMemberMacro(m_vao);
    DestructObjectMemberMacro(m_vbo);
    DestructObjectMemberMacro(m_iboVertex);
    DestructObjectMemberMacro(m_iboLine);
    DestructObjectMemberMacro(m_iboTriangle);
    DestructObjectMemberMacro(m_shaderProgram);
}

void OpenGLMapper::Render(Actor* actor)
{
    if (m_dirty)
    {
        BuildBufferObjects();
        BuildShaderProgram(actor);

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
    ConstructObjectMemberMacro(m_vao, OpenGLVertexArrayObject);
    ConstructObjectMemberMacro(m_vbo, OpenGLVertexBufferObject);
    m_vao->Bind();
    m_vbo->Bind();
    m_vbo->UpLoad(m_geometry->GetPoints()->GetPoints());
    m_vao->VertexAttrib();
    m_vao->UnBind();

    if (!m_geometry->GetCells()->GetVertexIndices().empty())
    {
        ConstructObjectMemberMacro(m_iboVertex, OpenGLIndexBufferObject);
        m_iboVertex->Bind();
        m_iboVertex->UpLoad(m_geometry->GetCells()->GetVertexIndices());
        m_iboVertex->UnBind();
    }
    if (!m_geometry->GetCells()->GetLineIndices().empty())
    {

        ConstructObjectMemberMacro(m_iboLine, OpenGLIndexBufferObject);
        m_iboLine->Bind();
        m_iboLine->UpLoad(m_geometry->GetCells()->GetLineIndices());
        m_iboLine->UnBind();
    }
    if (!m_geometry->GetCells()->GetTriangleIndices().empty())
    {
        ConstructObjectMemberMacro(m_iboTriangle, OpenGLIndexBufferObject);
        m_iboTriangle->Bind();
        m_iboTriangle->UpLoad(m_geometry->GetCells()->GetTriangleIndices());
        m_iboTriangle->UnBind();
    }
}

void OpenGLMapper::BuildShaderProgram(Actor* actor)
{
    ConstructObjectMemberMacro(m_shaderProgram, OpenGLShaderProgram);
    m_shaderProgram->SetAttribute();
    m_shaderProgram->ReplaceColor(actor->GetProperty()->GetColor());
}