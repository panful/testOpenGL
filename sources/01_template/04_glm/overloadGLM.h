
// glm重载流操作符

//------------------------------------------------------------------------------------------------
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

std::ostream& operator<<(std::ostream& os, const glm::vec2& vec)
{
    os << vec.x << '\t' << vec.y << '\n';
    return os;
}

std::ostream& operator<<(std::ostream& os, const glm::vec3& vec)
{
    os << vec.x << '\t' << vec.y << '\t' << vec.z << '\n';
    return os;
}

std::ostream& operator<<(std::ostream& os, const glm::vec4& vec)
{
    os << vec.x << '\t' << vec.y << '\t' << vec.z << '\t' << vec.w << '\n';
    return os;
}

//------------------------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& os, const glm::mat3& mat)
{
    os << mat[0].x << '\t' << mat[0].y << '\t' << mat[0].z << '\n' <<
        mat[1].x << '\t' << mat[1].y << '\t' << mat[1].z << '\n' <<
        mat[2].x << '\t' << mat[2].y << '\t' << mat[2].z << '\n';
    return os;
}

std::ostream& operator<<(std::ostream& os, const glm::mat4& mat)
{
    os << mat[0].x << '\t' << mat[0].y << '\t' << mat[0].z << '\t' << mat[0].w << '\n' <<
        mat[1].x << '\t' << mat[1].y << '\t' << mat[1].z << '\t' << mat[1].w << '\n' <<
        mat[2].x << '\t' << mat[2].y << '\t' << mat[2].z << '\t' << mat[2].w << '\n' <<
        mat[3].x << '\t' << mat[3].y << '\t' << mat[3].z << '\t' << mat[3].w << '\n';
    return os;
}

//------------------------------------------------------------------------------------------------