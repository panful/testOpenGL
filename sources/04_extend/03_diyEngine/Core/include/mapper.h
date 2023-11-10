#pragma once

#include "object.h"
#include <string>
#include <string_view>

class Geometry;
class Actor;
class LookupTable;

class Mapper : public Object
{
public:
    enum class ColorMode
    {
        Point,
        Cell,
    };

public:
    static Mapper* New();

    void SetGeometry(Geometry*);
    virtual void Render(Actor*);

    void SetColorVisibility(bool);
    bool GetColorVisibility() const noexcept;

    void SetColorMode(ColorMode);
    void SetColorModeToPointData();
    void SetColorModeToCellData();

    void SetColorArray(std::string_view);
    std::string_view GetColorArray() const noexcept;

    void SetUseLookupTable(bool);
    bool GetUseLookupTable() const noexcept;

    void SetLookupTable(LookupTable*);
    LookupTable* GetLookupTable() const noexcept;

    void SetInterpolateColorBeforeMapping(bool);
    bool GetInterpolateColorBeforeMapping() const noexcept;
    void InterpolateColorBeforeMappingOn();
    void InterpolateColorBeforeMappingOff();

protected:
    Mapper();
    ~Mapper() override;

protected:
    Geometry* m_geometry { nullptr };
    LookupTable* m_lookupTable { nullptr };
    std::string m_colorArray { "" };
    ColorMode m_colorMode { ColorMode::Point };
    bool m_dirty { true };
    bool m_colorVisibility { false };
    bool m_useLookupTable { false };
    bool m_interpolateColorBeforeMapping { true };
};