#include "OpenGLMapper.h"
#include "OpenGLIndexBufferObject.h"
#include "OpenGLShaderProgram.h"
#include "OpenGLTexture.h"
#include "OpenGLTextureBufferObject.h"
#include "OpenGLVertexArrayObject.h"
#include "OpenGLVertexBufferObjectGroup.h"
#include "actor.h"
#include "cells.h"
#include "dataArray.h"
#include "geometry.h"
#include "log.h"
#include "new.h"
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
    for (auto elem : m_internalColorTextures)
    {
        DestructObjectMemberMacro(elem);
    }
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
        if (m_colorMode == ColorMode::Cell && m_internalColorTextures[PT_Point])
        {
            m_internalColorTextures[PT_Point]->Bind();
        }
        m_primitives[PT_Point]->Bind();
        glDrawElements(GL_POINTS, static_cast<GLsizei>(m_geometry->GetCells()->GetNumberOfVertices()), GL_UNSIGNED_INT, 0);
    }
    if (m_primitives[PT_Line])
    {
        if (m_colorMode == ColorMode::Cell && m_internalColorTextures[GL_LINES])
        {
            m_internalColorTextures[GL_LINES]->Bind();
        }
        m_primitives[PT_Line]->Bind();
        glDrawElements(GL_LINES, static_cast<GLsizei>(m_geometry->GetCells()->GetNumberOfLines() * 2), GL_UNSIGNED_INT, 0);
    }
    if (m_primitives[PT_Triangle])
    {
        if (m_colorMode == ColorMode::Cell && m_internalColorTextures[PT_Triangle])
        {
            m_internalColorTextures[PT_Triangle]->Bind();
        }
        m_primitives[PT_Triangle]->Bind();
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_geometry->GetCells()->GetNumberOfTriangles() * 3), GL_UNSIGNED_INT, 0);
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
    if (m_geometry->GetCells()->GetNumberOfVertices() > 0)
    {
        ConstructObjectMemberMacro(m_primitives[PT_Point], OpenGLIndexBufferObject);
        m_primitives[PT_Point]->Bind();
        m_primitives[PT_Point]->UpLoad(m_geometry->GetCells()->GetVertexIndices());
        m_primitives[PT_Point]->UnBind();
    }
    if (m_geometry->GetCells()->GetNumberOfLines() > 0)
    {
        ConstructObjectMemberMacro(m_primitives[PT_Line], OpenGLIndexBufferObject);
        m_primitives[PT_Line]->Bind();
        m_primitives[PT_Line]->UpLoad(m_geometry->GetCells()->GetLineIndices());
        m_primitives[PT_Line]->UnBind();
    }
    if (m_geometry->GetCells()->GetNumberOfTriangles() > 0)
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
        switch (m_colorMode)
        {
        case ColorMode::Point:
            m_shaderProgram->ReplacePointColor();
            break;
        case ColorMode::Cell:
            m_shaderProgram->ReplaceCellColor();
            break;
        [[unlikely]] default:
            LogWarn("Color mode is error");
            break;
        }
    }
    else
    {
        m_shaderProgram->ReplacePrimitiveColor(actor->GetProperty()->GetColor());
    }
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
    case ColorMode::Cell:
        if (auto dataArray = m_geometry->GetCellArray(m_colorArray))
        {
            auto numberOfComponents = (size_t)dataArray->GetNumberOfComponents();
            auto numberOfVertices   = m_geometry->GetCells()->GetNumberOfVertices();
            auto numberOfLines      = m_geometry->GetCells()->GetNumberOfLines();
            auto numberOfTriangles  = m_geometry->GetCells()->GetNumberOfTriangles();
            auto&& vecData          = dataArray->GetData();

            if (numberOfVertices > 0)
            {
                std::vector<float> tmpVec(vecData.cbegin(), vecData.cbegin() + numberOfVertices * numberOfComponents);
                BuildCellColorTextures(PT_Point, tmpVec);
            }
            if (numberOfLines > 0)
            {
                std::vector<float> tmpVec(vecData.cbegin() + numberOfVertices * numberOfComponents,
                    vecData.cbegin() + numberOfVertices * numberOfComponents + numberOfLines * numberOfComponents);
                BuildCellColorTextures(PT_Line, tmpVec);
            }
            if (numberOfTriangles > 0)
            {
                std::vector<float> tmpVec(
                    vecData.cbegin() + numberOfVertices * numberOfComponents + numberOfLines * numberOfComponents, vecData.cend());
                BuildCellColorTextures(PT_Triangle, tmpVec);
            }
        }
        break;
    [[unlikely]] default:
        LogWarn("Color mode is error");
        break;
    }
}

void OpenGLMapper::BuildCellColorTextures(PrimitiveTypes primitiveType, const std::vector<float>& data)
{
    if (m_internalColorTextures[primitiveType])
    {
        m_internalColorTextures[primitiveType]->Delete();
    }
    ConstructObjectMemberMacro(m_internalColorTextures[primitiveType], OpenGLTexture);

    SmartPointer<OpenGLTextureBufferObject> tbo = SmartPointer<OpenGLTextureBufferObject>::New();
    tbo->SetFormat(GL_RGB32F);
    tbo->Bind();
    tbo->UpLoad(data);
    tbo->UnBind();

    m_internalColorTextures[primitiveType]->SetTarget(GL_TEXTURE_BUFFER);
    m_internalColorTextures[primitiveType]->Bind();
    m_internalColorTextures[primitiveType]->LoadBuffer(tbo);
    m_internalColorTextures[primitiveType]->UnBind();
}