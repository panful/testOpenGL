#pragma once

#include "object.h"
#include <array>

enum class Representation
{
    POINTS,
    WIREFRAME,
    SURFACE,
};

enum class Interpolation
{
    FLAT,
    GOURAUD,
    PHONG,
    PBR,
};

class Property : public Object
{
public:
    static Property* New();

    virtual void Render();

    void SetColor(const std::array<double, 3>&);
    std::array<double, 3> GetColor() const noexcept;

    void SetEdgeColor(const std::array<double, 3>&);
    std::array<double, 3> GetEdgeColor() const noexcept;

    void SetOpacity(double);
    double GetOpacity() const noexcept;

    void SetPointSize(double);
    double GetPointSize() const noexcept;

    void SetLineWidth(double);
    double GetLineWidth() const noexcept;

    void SetReresentation(Representation);
    Representation GetReresentation() const noexcept;

    void SetEdgeVisibility(bool);
    bool GetEdgeVisibility() const noexcept;
    void EdgeVisibilityOn();
    void EdgeVisibilityOff();

protected:
    Property();
    ~Property() override;

protected:
    std::array<double, 3> m_color { 1., 1., 1. };
    std::array<double, 3> m_ambientColor { 1., 1., 1. };
    std::array<double, 3> m_diffuseColor { 1., 1., 1. };
    std::array<double, 3> m_specularColor { 1., 1., 1. };
    std::array<double, 3> m_edgeColor { 1., 1., 1. };

    double m_opacity { 1. };
    double m_ambient { 1. };
    double m_diffuse { 1. };
    double m_specular { 1. };
    double m_pointSize { 1. };
    double m_lineWidth { 1. };

    Representation m_representation { Representation::SURFACE };
    Interpolation m_shading { Interpolation::PHONG };

    bool m_edgeVisibility { false };
    bool m_renderPointsAsSpheres { false };
    bool m_renderLinesAsTubes { false };
};