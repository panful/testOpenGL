#pragma once

#include "Group.h"
#include <string>
#include <string_view>

class GraphicsPipeline : public Group
{
public:
    void Accept(Visitor& visitor) override;

    void SetShaderCode(const std::string_view vert, const std::string_view frag);
    void SetShaderCode(const std::string_view vert, const std::string_view frag, const std::string_view geom);

    void Compile();
    void Bind() const noexcept;
    void UnBind() const noexcept;

    uint32_t GetHandle() const noexcept;

private:
    std::string m_vertCode {};
    std::string m_fragCode {};
    std::string m_geomCode {};
    uint32_t m_shaderProgramHandle { 0 };
};
