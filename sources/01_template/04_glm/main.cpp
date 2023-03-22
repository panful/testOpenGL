/*
 * 1. 两个点生成一个坐标系，即两点确定一个面
 * 2. 向量 加法 点乘 叉乘 矩阵
 * 3. 2D空间向量的垂直向量 (就是2D向量绕原点旋转90°）
 * 4. 转置矩阵，逆矩阵
 * 5. 从矩阵中提取旋转、缩放、平移矩阵 https://blog.csdn.net/hunter_wwq/article/details/21473519/
 * 6. glm::scale rotate translate ortho perspective lookAt frustum
 */

#define TEST2

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
// glm中的向量都是列向量

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
        glm::mat3 m31(
            1, 2, 3,
            4, 5, 6,
            7, 8, 9);

        glm::mat3x3 m32(
            1, 2, 3,
            4, 5, 6,
            7, 8, 9);

        if (m31 == m32)
        {
            std::cout << "glm::mat3 == glm::mat3x3\n";
        }

        std::cout << "m31 :\n"
                  << m31
                  << "m32 :\n"
                  << m32
                  << '\n';
    }

    std::cout << "-------------------------------------------\n";

    // 矩阵和向量相乘
    {
        glm::mat3 m31(
            1, 2, 3,
            2, 3, 4,
            3, 4, 5);

        glm::vec3 v3(1, 2, 3);

        auto mv3 = m31 * v3;
        auto vm3 = v3 * m31;

        std::cout << "mat * vec :\n"
                  << mv3
                  << "vec * mat :\n"
                  << vm3
                  << '\n';

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

    {
        glm::mat4 m4(
            1, 2, 3, 4,
            2, 3, 4, 5,
            3, 4, 5, 6,
            4, 5, 6, 7);

        glm::vec4 v4(1, 2, 3, 4);

        auto mv4 = m4 * v4;
        auto vm4 = v4 * m4;

        std::cout << mv4 << vm4;
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
    glm::mat4 mat1 {
        1.f, 2.f, 3.f, 4.f,
        12.f, 16.f, 14.f, 5.f,
        11.f, 13.f, 15.f, 6.f,
        10.f, 9.f, 8.f, 7.f
    };

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
    std::cout << "----- v1 ----- \n"
              << v1 << '\n'; //
    std::cout << "----- v2 ----- \n"
              << v2 << '\n';
    std::cout << "----- v3 ----- \n"
              << v3 << '\n';
    std::cout << "----- v4 ----- \n"
              << v4 << '\n';
    std::cout << "----- v5 ----- \n"
              << v5 << '\n';
    std::cout << "----- v6 ----- \n"
              << v6 << '\n'; //
    std::cout << "----- v7 ----- \n"
              << v7 << '\n';
    std::cout << "----- v8 ----- \n"
              << v8 << '\n';
    std::cout << "----- v9 ----- \n"
              << v9 << '\n'; //
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

    glm::mat4 resultMat {
        m00,
        m10,
        m20,
        m30,
        m01,
        m11,
        m21,
        m31,
        m02,
        m12,
        m22,
        m32,
        0,
        0,
        0,
        1,
    };

    // 每一列的向量长度
    auto l0 = std::hypot(m00, m01, m02);
    auto l1 = std::hypot(m10, m11, m12);
    auto l2 = std::hypot(m20, m21, m22);

    glm::mat4 rotateMat {
        m00 / l0,
        m10 / l1,
        m20 / l2,
        0,
        m01 / l0,
        m11 / l1,
        m21 / l2,
        0,
        m02 / l0,
        m12 / l1,
        m22 / l2,
        0,
        0,
        0,
        0,
        1,
    };

    glm::mat4 translationMat {
        1,
        0,
        0,
        m30,
        0,
        1,
        0,
        m31,
        0,
        0,
        1,
        m32,
        0,
        0,
        0,
        1,
    };

    glm::mat4 scaleMat {
        l0,
        0,
        0,
        0,
        0,
        l1,
        0,
        0,
        0,
        0,
        l2,
        0,
        0,
        0,
        0,
        1,
    };

    std::cout << "-----------result mat:--------------\n"
              << resultMat << "-----------rotate mat:--------------\n"
              << rotateMat << "-----------scale mat:--------------\n"
              << scaleMat << "-----------translation mat:--------------\n"
              << translationMat << '\n';

    // 只有这个resultMat和定义的相同
    resultMat = scaleMat * rotateMat * translationMat;
    std::cout << "-----------result mat:--------------\n"
              << resultMat;

    resultMat = translationMat * scaleMat * rotateMat;
    std::cout << "-----------result mat:--------------\n"
              << resultMat;

    resultMat = translationMat * rotateMat * scaleMat;
    std::cout << "-----------result mat:--------------\n"
              << resultMat;

    resultMat = rotateMat * scaleMat * translationMat;
    std::cout << "-----------result mat:--------------\n"
              << resultMat;

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
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 viewMat = glm::lookAt(cameraPos, cameraTarget, cameraUp);

    std::cout << "--------------- origin mat\n"
              << originMat
              << "--------------- scale mat\n"
              << scaleMat
              << "--------------- translate mat\n"
              << translateMat
              << "--------------- rotate mat\n"
              << rotateMat
              << "--------------- ortho mat\n"
              << orthoMat
              << "--------------- perspective mat\n"
              << perspectiveMat
              << "--------------- view mat\n"
              << viewMat
              << "--------------- frustum mat\n"
              << frustumMat;

    return 0;
}

#endif // TEST6
