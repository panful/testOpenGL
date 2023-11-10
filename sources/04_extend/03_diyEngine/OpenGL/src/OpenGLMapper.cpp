#include "OpenGLMapper.h"
#include "OpenGLIndexBufferObject.h"
#include "OpenGLShaderProgram.h"
#include "OpenGLVertexArrayObject.h"
#include "OpenGLVertexBufferObjectGroup.h"
#include "actor.h"
#include "cells.h"
#include "dataArray.h"
#include "geometry.h"
#include "log.h"
#include "objectFactory.h"
#include "points.h"
#include "property.h"
#include "smartpointer.h"

StandardNewMacro(OpenGLMapper);

OpenGLMapper::OpenGLMapper()
{
    LogDebug("");
}

OpenGLMapper::~OpenGLMapper()
{
    LogDebug("");
    DestructObjectMemberMacro(m_vao);
    for (auto elem : m_primitives)
    {
        DestructObjectMemberMacro(elem);
    }
    DestructObjectMemberMacro(m_vbos);
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

    if (m_primitives[PT_Point])
    {
        m_primitives[PT_Point]->Bind();
        glDrawElements(GL_POINTS, static_cast<GLsizei>(m_geometry->GetCells()->GetVertexIndices().size()), GL_UNSIGNED_INT, 0);
    }
    if (m_primitives[PT_Line])
    {
        m_primitives[PT_Line]->Bind();
        glDrawElements(GL_LINES, static_cast<GLsizei>(m_geometry->GetCells()->GetLineIndices().size()), GL_UNSIGNED_INT, 0);
    }
    if (m_primitives[PT_Triangle])
    {
        m_primitives[PT_Triangle]->Bind();
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_geometry->GetCells()->GetTriangleIndices().size()), GL_UNSIGNED_INT, 0);
    }

    m_vao->UnBind();
}

void OpenGLMapper::BuildBufferObjects()
{
    ConstructObjectMemberMacro(m_vao, OpenGLVertexArrayObject);
    ConstructObjectMemberMacro(m_vbos, OpenGLVertexBufferObjectGroup);

    m_vao->Bind();

    if (auto points = m_geometry->GetPoints()) [[likely]]
    {
        m_vbos->CacheVertexAttribute(OpenGLVertexAttributes::Pos, points->GetPoints());
    }
    else [[unlikely]]
    {
        LogWarn("Points were nullptr");
    }

    BuildColorBufferObject();

    m_vao->UnBind();

    BuildAllIBOs();
}

void OpenGLMapper::BuildAllIBOs()
{
    if (!m_geometry->GetCells()->GetVertexIndices().empty())
    {
        ConstructObjectMemberMacro(m_primitives[PT_Point], OpenGLIndexBufferObject);
        m_primitives[PT_Point]->Bind();
        m_primitives[PT_Point]->UpLoad(m_geometry->GetCells()->GetVertexIndices());
        m_primitives[PT_Point]->UnBind();
    }
    if (!m_geometry->GetCells()->GetLineIndices().empty())
    {
        ConstructObjectMemberMacro(m_primitives[PT_Line], OpenGLIndexBufferObject);
        m_primitives[PT_Line]->Bind();
        m_primitives[PT_Line]->UpLoad(m_geometry->GetCells()->GetLineIndices());
        m_primitives[PT_Line]->UnBind();
    }
    if (!m_geometry->GetCells()->GetTriangleIndices().empty())
    {
        ConstructObjectMemberMacro(m_primitives[PT_Triangle], OpenGLIndexBufferObject);
        m_primitives[PT_Triangle]->Bind();
        m_primitives[PT_Triangle]->UpLoad(m_geometry->GetCells()->GetTriangleIndices());
        m_primitives[PT_Triangle]->UnBind();
    }
}

void OpenGLMapper::BuildShaderProgram(Actor* actor)
{
    ConstructObjectMemberMacro(m_shaderProgram, OpenGLShaderProgram);

    if (m_colorVisibility)
    {
        m_shaderProgram->ReplaceColor();
    }
    else
    {
        m_shaderProgram->ReplaceColor(actor->GetProperty()->GetColor());
    }

    m_shaderProgram->SetAttribute();
}

void OpenGLMapper::BuildColorBufferObject()
{
    switch (m_colorMode)
    {
    case ColorMode::Point:
        if (auto dataArray = m_geometry->GetPointArray(m_colorArray))
        {
            if (m_interpolateColorBeforeMapping)
            {
                if (3 == dataArray->GetNumberOfComponents())
                {
                    if (dataArray->GetNumberOfTuples() == m_geometry->GetNumberOfPoints()) [[likely]]
                    {
                        m_vbos->CacheVertexAttribute(OpenGLVertexAttributes::Color, dataArray->GetData());
                    }
                    else [[unlikely]]
                    {
                        LogWarn("Unequal number of colors and vertices");

                        // 深拷贝一份颜色数据，并将大小设置为和顶点个数一致
                        auto tmpData = dataArray->GetData();
                        tmpData.resize(m_geometry->GetNumberOfPoints() * 3);
                        m_vbos->CacheVertexAttribute(OpenGLVertexAttributes::Color, tmpData);
                    }
                }
                else
                {
                }
            }
            else
            {

            }
        }
        break;
    [[unlikely]] default:
        LogWarn("Color mode is error");
        break;
    }
}