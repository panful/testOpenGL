/*
 * 1. 两个点生成一个坐标系，即两点确定一个面
 * 2. 向量 加法 点乘 叉乘 矩阵 矩阵左乘右乘
 * 3. 2D空间向量的垂直向量 (就是2D向量绕原点旋转90°）
 * 4. 转置矩阵，逆矩阵
 * 5. 从矩阵中提取旋转、缩放、平移矩阵 https://blog.csdn.net/hunter_wwq/article/details/21473519/
 * 6. glm::scale rotate translate ortho perspective lookAt frustum
 */

// glm中的向量都是列向量

#define TEST7

#ifdef TEST1

#include "print_glm.h"

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

    // ！[0]
    glm::vec3 t = glm::normalize(glm::cross(r, s));
    return { glm::vec4(-s, 0.0f), glm::vec4(r, 0.0f), glm::vec4(-t, 0.0f), glm::vec4(v0.x, v0.y, v0.z, 1.0f) };
    //! [0]

    //! [1] 不太正确
    // s /= std::hypot(s.x, s.y, s.z);

    ////glm::vec3 t = r * s;
    // glm::vec3 t(fabs(r.y * s.z - r.z * s.y), fabs(r.z * s.x - r.x * s.z), fabs(r.x * s.y - r.y * s.x));

    // return glm::mat4(
    //     r.x, s.x, t.x, 0.0,
    //     r.y, s.y, t.y, 0.0,
    //     r.z, s.z, t.z, 0.0,
    //     v0.x, v0.y, v0.z, 1.0f);
    //![1]
}

int main()
{
    // 两个正交向量的叉乘会产生一个与这两个向量都垂直的第三个向量，一起可以组成一个直角坐标系

    auto ret1 = genMatrix({ 1, 0, 0 }, { 0, 1, 0 });
    std::cout << ret1;

    std::cout << "--------------------------------------\n";

    auto ret2 = genMatrix({ 1, 0, 0 }, { 0, 0, 1 });
    std::cout << ret2;

    return 0;
}

#endif // TEST1

#ifdef TEST2

#include "print_glm.h"

// 在glm中 * 既不是点乘也不是叉乘

// 点乘
//
// ||A·B|| = ||A|| * ||B|| * cosθ
//
// 结果>0: 夹角为 0-90度，  a，b向量方向相同
// 结果<0: 夹角为 90-180度，a，b向量方向相反
// 结果=0: 夹角为 90度，    a，b向量方向垂直

// 叉乘，仅可用于3维向量
//
// ||AxB|| = ||A|| * ||B|| * sinθ
//
// ---   ---   -----------
// |a|   |x|   |b*z - c*y|
// |b| X |y| = |c*x - a*z|
// |c|   |z|   |a*y - b*x|
// ---   ---   -----------

int main()
{
    // 获取向量的指针
    {
        glm::vec3 v31(1, 2, 3);
        float* ptr = glm::value_ptr(v31);
        std::cout << "glm::vec3(1, 2, 3) = \t" << ptr[0] << ',' << ptr[1] << ',' << ptr[2] << '\n';
    }

    std::cout << "-------------------------------------------\n";

    // 2维向量的点乘，叉乘
    {
        glm::vec2 v21(2, 3);
        glm::vec2 v22(4, 5);

        auto add_vec = v21 + v22;

        auto multi_vec1 = v21 * v22;
        auto multi_vec2 = v22 * v21;

        // 向量的点乘结果是一个数量（一维）
        auto dot_vec1 = glm::dot(v21, v22);
        auto dot_vec2 = glm::dot(v22, v21);

        // glm::cross(v21, v22); // 2维向量无法使用glm::cross()

        std::cout << "( 2,3 ) + ( 4,5 ) = \t" << add_vec << '\n';
        std::cout << "( 2,3 ) * ( 4,5 ) = \t" << multi_vec1 << '\n';
        std::cout << "( 4,5 ) * ( 2,3 ) = \t" << multi_vec2 << '\n';
        std::cout << "( 2,3 ) . ( 4,5 ) = \t" << dot_vec1 << '\n';
        std::cout << "( 4,5 ) . ( 2,3 ) = \t" << dot_vec2 << '\n';
    }

    std::cout << "-------------------------------------------\n";

    // 3维向量的点乘，叉乘
    {
        glm::vec3 v32(1, 2, 3);
        glm::vec3 v33(3, 4, 5);

        // 叉乘
        auto cross_vec1 = glm::cross(v32, v33);
        auto cross_vec2 = glm::cross(v33, v32);

        // 点乘
        auto dot_vec1 = glm::dot(v32, v33);
        auto dot_vec2 = glm::dot(v33, v32);

        auto multi_vec1 = v32 * v33;
        auto multi_vec2 = v33 * v32;

        std::cout << "(1, 2, 3) x (3, 4, 5) = \t" << cross_vec1 << '\n';
        std::cout << "(3, 4, 5) x (1, 2, 3) = \t" << cross_vec2 << '\n';
        std::cout << "(1, 2, 3) * (3, 4, 5) = \t" << multi_vec1 << '\n';
        std::cout << "(3, 4, 5) * (1, 2, 3) = \t" << multi_vec2 << '\n';
        std::cout << "(1, 2, 3) . (3, 4, 5) = \t" << dot_vec1 << '\n';
        std::cout << "(3, 4, 5) . (1, 2, 3) = \t" << dot_vec2 << '\n';
    }

    std::cout << "-------------------------------------------\n";

    // glm::mat3和glm::mat3x3等价，glm::mat4和glm::mat4x4等价
    {
        glm::mat3 m31(1, 2, 3, 4, 5, 6, 7, 8, 9);

        glm::mat3x3 m32(1, 2, 3, 4, 5, 6, 7, 8, 9);

        if (m31 == m32)
        {
            std::cout << "glm::mat3 == glm::mat3x3\n";
        }

        std::cout << "m31 :\n" << m31 << "m32 :\n" << m32 << '\n';
    }

    std::cout << "-------------------------------------------\n";

    // 矩阵和向量相乘
    // 向量在前和在后是没有区别的
    {
        glm::mat3 m31(1, 2, 3, 2, 3, 4, 3, 4, 5);

        glm::vec3 v3(3, 6, 9);

        auto mv3 = m31 * v3;
        auto vm3 = v3 * m31;

        std::cout << "mat * vec :\n" << mv3 << "vec * mat :\n" << vm3 << '\n';

        if (mv3 == vm3)
        {
            std::cout << "mat * vec == vec * mat\n";
        }
        else
        {
            std::cout << "mat * vec != vec * mat\n";
        }
    }

    std::cout << "-------------------------------------------\n";

    // 矩阵的左乘和右乘结果是不一样的
    {
        glm::mat3 m31(1, 2, 3, 4, 5, 6, 7, 8, 9);

        glm::mat3 m32(3, 6, 9, 2, 5, 8, 1, 4, 7);

        std::cout << "m31 * m32 = \n" << m31 * m32 << "m32 * m31 = \n" << m32 * m31;
    }

    return 0;
}

#endif // TEST2

#ifdef TEST3

#include "print_glm.h"

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

#include "print_glm.h"

int main()
{
    glm::vec4 v1 { 1.1, 2.2, 3.3, 0. };
    glm::mat4 mat1 { 1.f, 2.f, 3.f, 4.f, 12.f, 16.f, 14.f, 5.f, 11.f, 13.f, 15.f, 6.f, 10.f, 9.f, 8.f, 7.f };

    // 逆矩阵 AB=BA=E 则称B是A的逆矩阵
    auto inverseMat = glm::inverse(mat1);

    // 转置矩阵 将矩阵的行列互换
    auto transposeMat = glm::transpose(mat1);

    auto v2 = inverseMat * v1;
    auto v3 = v1 * inverseMat;
    auto v4 = transposeMat * v1;
    auto v5 = v1 * transposeMat;

    auto v6 = v2 * transposeMat;
    auto v7 = v3 * transposeMat;
    auto v8 = transposeMat * v2;
    auto v9 = transposeMat * v3;

    auto v10 = v1 * mat1 * glm::transpose(mat1);
    auto v11 = mat1 * v1 * glm::transpose(mat1);

    auto mat2 = mat1 * glm::transpose(mat1);
    auto mat3 = mat1 * glm::inverse(mat1);

    // 1 和 6 以及 9 相等
    std::cout << "----- v1 ----- \n" << v1 << '\n'; //
    std::cout << "----- v2 ----- \n" << v2 << '\n';
    std::cout << "----- v3 ----- \n" << v3 << '\n';
    std::cout << "----- v4 ----- \n" << v4 << '\n';
    std::cout << "----- v5 ----- \n" << v5 << '\n';
    std::cout << "----- v6 ----- \n" << v6 << '\n'; //
    std::cout << "----- v7 ----- \n" << v7 << '\n';
    std::cout << "----- v8 ----- \n" << v8 << '\n';
    std::cout << "----- v9 ----- \n" << v9 << '\n'; //
}

#endif // TEST4

#ifdef TEST5

#include "print_glm.h"

int main()
{
    double m00 = 1.;
    double m01 = 2.;
    double m02 = 3.;

    double m10 = 4.;
    double m11 = 5.;
    double m12 = 6.;

    double m20 = 7.;
    double m21 = 8.;
    double m22 = 9.;

    double m30 = 3.;
    double m31 = 6.;
    double m32 = 9.;

    // clang-format off
    glm::mat4 resultMat {
        m00, m10, m20, m30,
        m01, m11, m21, m31,
        m02, m12, m22, m32,
          0,   0,   0,   1,
    };
    // clang-format on

    // 每一列的向量长度
    auto l0 = std::hypot(m00, m01, m02);
    auto l1 = std::hypot(m10, m11, m12);
    auto l2 = std::hypot(m20, m21, m22);

    // clang-format off
    glm::mat4 rotateMat {
        m00 / l0, m10 / l1, m20 / l2, 0,
        m01 / l0, m11 / l1, m21 / l2, 0,
        m02 / l0, m12 / l1, m22 / l2, 0,
               0,        0,        0, 1,
    };

    glm::mat4 translationMat {
        1, 0, 0, m30,
        0, 1, 0, m31,
        0, 0, 1, m32,
        0, 0, 0, 1,
    };

    glm::mat4 scaleMat {
        l0,  0,  0, 0,
         0, l1,  0, 0,
         0,  0, l2, 0,
         0,  0,  0, 1,
    };
    // clang-format on

    std::cout << "-----------result mat:--------------\n"
              << resultMat << "-----------rotate mat:--------------\n"
              << rotateMat << "-----------scale mat:--------------\n"
              << scaleMat << "-----------translation mat:--------------\n"
              << translationMat << '\n';

    // 先缩放再旋转最后再平移
    resultMat = scaleMat * rotateMat * translationMat;
    std::cout << "-----------result mat:--------------\n" << resultMat;

    return 0;
}

#endif // TEST5

#ifdef TEST6

#include "print_glm.h"

int main()
{
    glm::mat4 originMat = glm::mat4(1.0);

    // 不会对originMat修改，结果是返回值
    // 是对originMat右乘
    // x缩放为原来的0.5，y缩放为原来的2.0，z不变
    auto scaleMat = glm::scale(originMat, glm::vec3(0.5, 2.0, 1.0));

    // 绕z轴顺时针旋转45°
    auto rotateMat = glm::rotate(originMat, glm::radians(-45.0f), glm::vec3(0., 0., 1.));

    // +x平移1个单位，+y平移3个单位，-z平移5个单位
    auto translateMat = glm::translate(originMat, glm::vec3(1.0, 3.0, -5.0));

    // 正交投影矩阵
    // 这将创建一个正交投影矩阵，其左、右、下、上、近、远平面的位置分别为 (0, 800, 0, 600, -1, 1)。
    // 这个矩阵可以用于渲染一个800x600像素的2D场景。
    // 观察坐标的x属于[0,800] y属于[0,600] z属于[-1,1]才可以显示，不是模型坐标也不是世界坐标
    // 注意视口的宽高可能导致物体拉伸
    auto orthoMat = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);

    // 透视投影矩阵
    // 这将创建一个透视投影矩阵，其视角为45度，宽高比为800:600，近平面距离为0.1，远平面距离为100。
    // 近远平面可以理解为与摄像机的距离，应该为正值，且近平面小于远平面
    // 这个矩阵可以用于渲染一个视锥体场景，其中近处物体看起来比远处物体更大。
    auto perspectiveMat = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    // 透视投影矩阵
    // 六个参数分别为左、右、下、上、近、远平面
    // 可以用于创建非对称的投影矩阵，一般较少使用
    auto frustumMat = glm::frustum(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);

    // 观察(视图）矩阵
    // 创建一个视图矩阵，其中相机的位置为 (0, 0, 3)，相机朝向的目标位置为 (0, 0, 0)，相机的上方向为 (0, 1, 0)。
    // 这个矩阵可以用于渲染一个相机观察的3D场景，将场景中的物体坐标转换为相机坐标。
    glm::vec3 cameraPos    = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp     = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 viewMat      = glm::lookAt(cameraPos, cameraTarget, cameraUp);

    // glm::lookAt()   返回的是一个在右手坐标系的矩阵，对一个坐标变换后Z值的符号是反的
    // glm::lookAtLH();
    // glm::lookAtRH();

    std::cout << "--------------- origin mat\n"
              << originMat << "--------------- scale mat\n"
              << scaleMat << "--------------- translate mat\n"
              << translateMat << "--------------- rotate mat\n"
              << rotateMat << "--------------- ortho mat\n"
              << orthoMat << "--------------- perspective mat\n"
              << perspectiveMat << "--------------- view mat\n"
              << viewMat << "--------------- frustum mat\n"
              << frustumMat;

    return 0;
}

#endif // TEST6

#ifdef TEST7

// #define GLM_FORCE_DEPTH_ZERO_TO_ONE 只是将深度值限制在[0,1]，并不会改变坐标系
#include "print_glm.h"
#include <array>
#include <cmath>
#include <iostream>

using Mat3 = std::array<std::array<float, 3>, 3>;
using Vec3 = std::array<float, 3>;

Vec3 Cross(const Vec3& x, const Vec3& y)
{
    return { x[1] * y[2] - y[1] * x[2], x[2] * y[0] - y[2] * x[0], x[0] * y[1] - y[0] * x[1] };
}

float Dot(const Vec3& u, const Vec3& v)
{
    return u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
}

Vec3 Normalize(const Vec3& v)
{
    const auto d = std::hypot(v[0], v[1], v[2]);
    return { v[0] / d, v[1] / d, v[2] / d };
}

Mat3 rotate(float angle, const Vec3& v)
{
    const auto a = angle;
    const auto c = cos(a);
    const auto s = sin(a);

    Vec3 axis { Normalize(v) };
    Vec3 temp { (1.f - c) * axis[0], (1.f - c) * axis[1], (1.f - c) * axis[2] };

    Mat3 Rotate {};
    Rotate[0][0] = c + temp[0] * axis[0];
    Rotate[0][1] = temp[0] * axis[1] + s * axis[2];
    Rotate[0][2] = temp[0] * axis[2] - s * axis[1];

    Rotate[1][0] = temp[1] * axis[0] - s * axis[2];
    Rotate[1][1] = c + temp[1] * axis[1];
    Rotate[1][2] = temp[1] * axis[2] + s * axis[0];

    Rotate[2][0] = temp[2] * axis[0] + s * axis[1];
    Rotate[2][1] = temp[2] * axis[1] - s * axis[0];
    Rotate[2][2] = c + temp[2] * axis[2];

    return Rotate;
}

Vec3 matrixVectorMultiply(const Mat3& matrix, const Vec3& vector)
{
    Vec3 result = { 0.0f, 0.0f, 0.0f };
    for (size_t i = 0; i < 3; ++i)
    {
        for (size_t j = 0; j < 3; ++j)
        {
            result[i] += matrix[j][i] * vector[j];
        }
    }
    return result;
}

int main()
{
    // 观察矩阵
    if (0)
    {
        // 默认情况下 glm::lookAt <=> glm::lookAtRH
        auto lookAt = glm::lookAt(glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        std::cout << lookAt << '\n';

        auto lookAtLH = glm::lookAtLH(glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        std::cout << lookAtLH << '\n';

        auto lookAtRH = glm::lookAtRH(glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        std::cout << lookAtRH << '\n';
    }

    // 投影矩阵
    if (0)
    {
        auto ortho = glm::ortho(-100.f, 100.f, -100.f, 100.f, -1.f, 1.f);
        std::cout << ortho << '\n';

        auto perspective = glm::perspective(glm::radians(45.f), 8.f / 6.f, .1f, 100.f);
        std::cout << perspective << '\n';

        auto frustum = glm::frustum(-100.f, 100.f, -100.f, 100.f, -1.f, 1.f);
        std::cout << frustum << '\n';
    }

    // 向量的 distance length std::hypot 点与点之间的距离
    if (0)
    {
        glm::vec3 p1 { 1.f, 2.f, 3.f };
        glm::vec3 p2 { 3.f, 2.f, 1.f };

        auto length   = glm::length(glm::vec3 { p2.x - p1.x, p2.y - p1.y, p2.z - p1.z });
        auto distance = glm::distance(p2, p1);
        auto hypot    = std::hypot(p2.x - p1.x, p2.y - p1.y, p2.z - p1.z);
        std::cout << length << '\t' << distance << '\t' << hypot << '\n';
    }

    // 旋转的方向
    if (0)
    {
        // 眼睛看向的方向和旋转轴指向的方向一致，逆时针旋转
        glm::vec3 axis { 0.f, 0.f, 1.f };
        glm::vec4 pt { 1.f, 0.f, 0.f, 1.f };

        glm::mat4 mat = glm::rotate(glm::mat4(1.f), glm::radians(-30.f), axis);
        auto p        = mat * pt;
        std::cout << p << '\n';
    }

    // 叉乘的交换律
    if (0)
    {
        glm::vec3 x { 1.f, 0.f, 0.f };
        glm::vec3 y { 0.f, -1.f, 0.f };
        glm::vec3 z { 0.f, 0.f, 1.f };
        auto r1 = glm::cross(x, y);
        auto r2 = glm::cross(y, x);
        auto r3 = glm::cross(y, z);
        auto r4 = glm::cross(z, y);
        auto r5 = glm::cross(x, z);
        auto r6 = glm::cross(z, x);
        std::cout << r1 << r2 << r3 << r4 << r5 << r6 << '\n';
    }

    // 向量归一化
    if (0)
    {
        glm::vec3 p1 { 1.f, 2.f, 2.f };
        glm::vec3 p2 { 3.f, 3.f, 2.f };

        auto dir = glm::normalize(p2 - p1);
        auto dis = glm::distance(p2, p1);

        auto x = p2[0] - p1[0];
        auto y = p2[1] - p1[1];
        auto z = p2[2] - p1[2];
        auto d = std::hypot(x, y, z);

        x /= d;
        y /= d;
        z /= d;

        std::cout << x << '\t' << y << '\t' << z << '\n' << dir << '\n' << dis << '\t' << d << '\n';

        auto n = Normalize({ 2.f, 1.f, 0.f });
        std::cout << n[0] << ' ' << n[1] << ' ' << n[2] << '\n';
    }

    // 矩阵乘法的顺序
    if (0)
    {
        glm::mat4 m1(1.f, 2.f, 3.f, 4.f, 2.f, 2.f, 3.f, 4.f, 3.f, 2.f, 3.f, 4.f, 4.f, 2.f, 3.f, 4.f);
        glm::mat4 m2(2.f, 2.f, 3.f, 5.f, 3.f, 2.f, 3.f, 5.f, 4.f, 2.f, 3.f, 5.f, 5.f, 2.f, 3.f, 5.f);
        glm::vec4 v(1.f, 2.f, 3.f, 4.f);

        auto x = m1 * v;
        auto y = m2 * x;

        std::cout << y;

        auto z = m2 * m1 * v;
        std::cout << z;
    }

    // 矩阵的乘法
    if (0)
    {
        std::array<float, 16> matrix1 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
        std::array<float, 16> matrix2 = { 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 };
        std::array<float, 16> matrix3 = { 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28 };

        auto multi = [](const std::array<float, 16>& mat1, const std::array<float, 16>& mat2)
        {
            std::array<float, 16> result;
            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 4; ++j)
                {
                    result[i * 4 + j] = 0;
                    for (int k = 0; k < 4; ++k)
                    {
                        result[i * 4 + j] += mat1[i * 4 + k] * mat2[k * 4 + j];
                    }
                }
            }

            return result;
        };

        // auto result = multi(multi(matrix1, matrix2), matrix3);
        auto result = multi(matrix1, multi(matrix2, matrix3));

        std::cout << "Result matrix:" << std::endl;
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                std::cout << result[i * 4 + j] << '\t';
            }
            std::cout << std::endl;
        }

        std::cout << "-------------------\n";

        auto res = glm::make_mat4(matrix3.data()) * glm::make_mat4(matrix2.data()) * glm::make_mat4(matrix1.data());
        std::cout << res;
    }

    // 计算一个方向旋转到另一个方向的矩阵
    if (1)
    {
        std::array<float, 3> v1 = { 0.0f, 1.0f, 0.0f }; // 向量v1
        std::array<float, 3> v2 = { 1.0f, 1.0f, 0.0f }; // 向量v2

        auto axis   = Normalize(Cross(v1, v2));
        auto ctheta = Dot(Normalize(v1), Normalize(v2));
        auto theta  = std::acos(ctheta);

        auto mat = rotate(theta, axis);

        auto rv = matrixVectorMultiply(mat, v1);
        std::cout << rv[0] << '\t' << rv[1] << '\t' << rv[2] << '\n';
    }
}

#endif // TEST7
