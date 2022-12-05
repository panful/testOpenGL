/*
* 1. 两个点生成一个坐标系，即两点确定一个面
* 2. 向量 加法 点乘 叉乘 矩阵
* 3. 2D空间向量的垂直向量 (就是2D向量绕原点旋转90°）
* 4. 转置矩阵，逆矩阵
*/

#define TEST4

#ifdef TEST1

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// 两个点生成一个坐标系
glm::mat4 genMatrix(glm::vec3 v0, glm::vec3 v1)
{
    glm::vec3 r(v1.x - v0.x, v1.y - v0.y, v1.z - v0.z);
    glm::vec3 s(1.0f, 0.0f, 0.0f);

    if (fabs(r.x) <= fabs(r.y) && fabs(r.x) <= fabs(r.z))
    {
        s.x = 0.0f;
        s.y = -r.z;
        s.z = r.y;
    }
    else if (fabs(r.y) <= fabs(r.x) && fabs(r.y) <= fabs(r.z))
    {
        s.x = -r.z;
        s.y = 0.0f;
        s.z = r.x;
    }
    else if (fabs(r.z) <= fabs(r.x) && fabs(r.z) <= fabs(r.y))
    {
        s.x = -r.y;
        s.y = r.x;
        s.z = 0.0f;
    }

    //！[0]
    glm::vec3 t = glm::normalize(glm::cross(r, s));
    return { glm::vec4(-s, 0.0f), glm::vec4(r, 0.0f), glm::vec4(-t, 0.0f), glm::vec4(v0.x, v0.y, v0.z, 1.0f) };
    //! [0]

    //! [1] 不太正确
        //s /= std::hypot(s.x, s.y, s.z);

        ////glm::vec3 t = r * s;
        //glm::vec3 t(fabs(r.y * s.z - r.z * s.y), fabs(r.z * s.x - r.x * s.z), fabs(r.x * s.y - r.y * s.x));

        //return glm::mat4(
        //	r.x, s.x, t.x, 0.0,
        //	r.y, s.y, t.y, 0.0,
        //	r.z, s.z, t.z, 0.0,
        //	v0.x, v0.y, v0.z, 1.0f);
    //![1]
}

int main()
{
    auto ret1 = genMatrix({ 1,0,0 }, { 0,1,0 });
    std::cout << ret1[0].x << ',' << ret1[0].y << ',' << ret1[0].z << '\n';
    std::cout << ret1[1].x << ',' << ret1[1].y << ',' << ret1[1].z << '\n';
    std::cout << ret1[2].x << ',' << ret1[2].y << ',' << ret1[2].z << '\n';

    std::cout << "================\n";

    auto ret2 = genMatrix({ 1,0,0 }, { 0,0,1 });
    std::cout << ret2[0].x << ',' << ret2[0].y << ',' << ret2[0].z << '\n';
    std::cout << ret2[1].x << ',' << ret2[1].y << ',' << ret2[1].z << '\n';
    std::cout << ret2[2].x << ',' << ret2[2].y << ',' << ret2[2].z << '\n';
}

#endif // TEST1

#ifdef TEST2

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int main()
{
    // 在glm中 * 既不是点乘也不是叉乘

    glm::vec3 v31(1, 2, 3);
    // 获取向量的指针
    float* ret1 = glm::value_ptr(v31);
    std::cout << "glm::vec3(1, 2, 3) = \t" << ret1[0] << ',' << ret1[1] << ',' << ret1[2] << '\n';

    std::cout << "-----------------\n";

    glm::vec2 v21(2, 3);
    glm::vec2 v22(4, 5);
    glm::vec2 ret2 = v21 + v22;
    glm::vec2 ret3 = v21 * v22;
    float ret4 = glm::dot(v21, v22); // 向量的点乘结果是一个数量
    // glm::cross(v21, v22); // glm中2维向量无法使用glm::cross()

    std::cout << "( 2,3 ) + ( 4,5 ) = \t" << ret2.x << ',' << ret2.y << '\n';
    std::cout << "( 2,3 ) * ( 4,5 ) = \t" << ret3.x << ',' << ret3.y << '\n';
    std::cout << "( 2,3 ) . ( 4,5 ) = \t" << ret4 << '\n';

    std::cout << "-----------------\n";

    glm::vec3 v32(1, 0, 0);
    glm::vec3 v33(0, 0, 1);
    glm::vec3 ret5 = glm::cross(v32, v33); // 叉乘
    glm::vec3 ret6 = v32 * v33;
    float ret7 = glm::dot(v32, v33);       // 点乘

    std::cout << "(1, 0, 0) x (0, 0, 1) = \t" << ret5.x << ',' << ret5.y << ',' << ret5.z << '\n';
    std::cout << "(1, 0, 0) * (0, 0, 1) = \t" << ret6.x << ',' << ret6.y << ',' << ret6.z << '\n';
    std::cout << "(1, 0, 0) . (0, 0, 1) = \t" << ret7 << '\n';

    std::cout << "-----------------\n";

    // glm::mat4 == glm::mat4x4
    glm::mat4 m41(1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f);

    glm::mat4x4 m42(1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f);

    if (m42 == m41)
    {
        std::cout << "glm::mat4 == glm::mat4x4\n";
    }

    glm::mat3 m3(1, 2, 3,
        4, 5, 6,
        7, 8, 9);

    glm::mat3 m4(3, 2, 1,
        6, 5, 4,
        9, 8, 7);

    glm::vec3 ret8 = m3 * v31;
    glm::vec3 ret9 = v31 * m3;
    if (ret8 == ret9)
    {
        std::cout << " m3 * v31 == v31 * m3 \n";
    }
    glm::mat3 ret10 = m3 * m4;

    return 0;
}

#endif // TEST2

#ifdef TEST3

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// type为true则垂直向量在原向量右边，否则在左边
glm::vec2 GenVec(glm::vec2 vec, int type)
{
    if (type)
    {
        return glm::mat2(0, -1, 1, 0) * vec;
    }
    else
    {
        return glm::mat2(0, 1, -1, 0) * vec;
    }
}

int main()
{
    auto ret1 = GenVec(glm::vec2(0, 1), 0);
    auto ret2 = GenVec(glm::vec2(0, 1), 1);

    auto ret3 = GenVec(glm::vec2(1, 0), 0);
    auto ret4 = GenVec(glm::vec2(1, 0), 1);

    auto ret5 = GenVec(glm::vec2(1, 1), 0);
    auto ret6 = GenVec(glm::vec2(1, 1), 1);
}

#endif // TEST3

#ifdef TEST4

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int main()
{
    glm::vec4 v1{ 1.1,2.2,3.3,0. };
    glm::mat4 mat1{
        1.f, 2.f, 3.f, 4.f,
        12.f, 16.f, 14.f, 5.f,
        11.f, 13.f, 15.f, 6.f,
        10.f, 9.f, 8.f, 7.f };

    auto inverseMat = glm::inverse(mat1);
    auto transposeMat = glm::transpose(mat1);

    // 逆矩阵
    auto v2 = inverseMat * v1;
    auto v3 = v1 * inverseMat;
    auto v4 = transposeMat * v1;
    auto v5 = v1 * transposeMat;

    // 转置矩阵
    auto v6 = v2 * transposeMat;
    auto v7 = v3 * transposeMat;
    auto v8 = transposeMat * v2;
    auto v9 = transposeMat * v3;

    // 1 和 6 以及 9 相等
    std::cout << v1.x << ',' << v1.y << ',' << v1.z << ',' << v1.w<< '\n';  //
    std::cout << v2.x << ',' << v2.y << ',' << v2.z << ',' << v2.w<< '\n';
    std::cout << v3.x << ',' << v3.y << ',' << v3.z << ',' << v3.w << '\n';
    std::cout << v4.x << ',' << v4.y << ',' << v4.z << ',' << v4.w << '\n';
    std::cout << v5.x << ',' << v5.y << ',' << v5.z << ',' << v5.w << '\n';
    std::cout << v6.x << ',' << v6.y << ',' << v6.z << ',' << v6.w << '\n'; //
    std::cout << v7.x << ',' << v7.y << ',' << v7.z << ',' << v7.w << '\n';
    std::cout << v8.x << ',' << v8.y << ',' << v8.z << ',' << v8.w << '\n';
    std::cout << v9.x << ',' << v9.y << ',' << v9.z << ',' << v9.w << '\n'; //
}

#endif // TEST4
