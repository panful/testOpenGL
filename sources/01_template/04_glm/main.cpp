/*
* 1. 两个点生成一个坐标系，即两点确定一个面
* 2. 向量 加法 点乘 叉乘 矩阵
* 3. 2D空间向量的垂直向量 (就是2D向量绕原点旋转90°）
* 4. 转置矩阵，逆矩阵
* 5. 从矩阵中提取旋转、缩放、平移矩阵 https://blog.csdn.net/hunter_wwq/article/details/21473519/
* 6. 变换矩阵：缩放、平移、旋转，投影矩阵：正交、透视，观察（视图）矩阵
*/

#define TEST6

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

    //！[0]
    glm::vec3 t = glm::normalize(glm::cross(r, s));
    return { glm::vec4(-s, 0.0f), glm::vec4(r, 0.0f), glm::vec4(-t, 0.0f), glm::vec4(v0.x, v0.y, v0.z, 1.0f) };
    //! [0]

    //! [1] 不太正确
        //s /= std::hypot(s.x, s.y, s.z);

        ////glm::vec3 t = r * s;
        //glm::vec3 t(fabs(r.y * s.z - r.z * s.y), fabs(r.z * s.x - r.x * s.z), fabs(r.x * s.y - r.y * s.x));

        //return glm::mat4(
        //    r.x, s.x, t.x, 0.0,
        //    r.y, s.y, t.y, 0.0,
        //    r.z, s.z, t.z, 0.0,
        //    v0.x, v0.y, v0.z, 1.0f);
    //![1]
}

int main()
{
    // 两个正交向量的叉乘会产生一个与这两个向量都垂直的第三个向量，一起可以组成一个直角坐标系

    auto ret1 = genMatrix({ 1,0,0 }, { 0,1,0 });
    std::cout << ret1;

    std::cout << "--------------------------------------\n";

    auto ret2 = genMatrix({ 1,0,0 }, { 0,0,1 });
    std::cout << ret2;

    return 0;
}

#endif // TEST1

#ifdef TEST2

#include "print_glm.h"

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

    std::cout << "( 2,3 ) + ( 4,5 ) = \t" << ret2 << '\n';
    std::cout << "( 2,3 ) * ( 4,5 ) = \t" << ret3 << '\n';
    std::cout << "( 2,3 ) . ( 4,5 ) = \t" << ret4 << '\n';

    std::cout << "-----------------\n";

    glm::vec3 v32(1, 0, 0);
    glm::vec3 v33(0, 0, 1);
    glm::vec3 ret5 = glm::cross(v32, v33); // 叉乘
    glm::vec3 ret6 = v32 * v33;
    float ret7 = glm::dot(v32, v33);       // 点乘

    std::cout << "(1, 0, 0) x (0, 0, 1) = \t" << ret5 << '\n';
    std::cout << "(1, 0, 0) * (0, 0, 1) = \t" << ret6 << '\n';
    std::cout << "(1, 0, 0) . (0, 0, 1) = \t" << ret7 << '\n';

    std::cout << "-----------------\n";

    // glm::mat4 == glm::mat4x4
    glm::mat4 m41(
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f);

    glm::mat4x4 m42(
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f);

    std::cout << m41 << m42;

    if (m42 == m41)
    {
        std::cout << "glm::mat4 == glm::mat4x4\n";
    }

    std::cout << "-----------------\n";

    glm::mat3 m3(
        1, 2, 3,
        4, 5, 6,
        7, 8, 9);

    glm::mat3 m4(
        3, 2, 1,
        6, 5, 4,
        9, 8, 7);

    glm::vec3 ret8 = m3 * v31;
    glm::vec3 ret9 = v31 * m3;

    std::cout << "m3 * v31:\n" << ret8 << "v31 * m3:\n" << ret9;

    if (ret8 == ret9)
    {
        std::cout << " m3 * v31 == v31 * m3 \n";
    }
    glm::mat3 ret10 = m3 * m4;

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
    glm::vec4 v1{ 1.1,2.2,3.3,0. };
    glm::mat4 mat1{
        1.f, 2.f, 3.f, 4.f,
        12.f, 16.f, 14.f, 5.f,
        11.f, 13.f, 15.f, 6.f,
        10.f, 9.f, 8.f, 7.f };

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

    auto v10 = v1* mat1* glm::transpose(mat1);
    auto v11 = mat1* v1* glm::transpose(mat1);

    auto mat2 = mat1 * glm::transpose(mat1);
    auto mat3 = mat1 * glm::inverse(mat1);

    // 1 和 6 以及 9 相等
    std::cout << "----- v1 ----- \n" <<v1<< '\n';  //
    std::cout << "----- v2 ----- \n" <<v2<< '\n';
    std::cout << "----- v3 ----- \n" <<v3<< '\n';
    std::cout << "----- v4 ----- \n" <<v4<< '\n';
    std::cout << "----- v5 ----- \n" <<v5<< '\n';
    std::cout << "----- v6 ----- \n" <<v6<< '\n'; //
    std::cout << "----- v7 ----- \n" <<v7<< '\n';
    std::cout << "----- v8 ----- \n" <<v8<< '\n';
    std::cout << "----- v9 ----- \n" <<v9<< '\n'; //
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

    glm::mat4 resultMat{
        m00, m10, m20, m30,
        m01, m11, m21, m31,
        m02, m12, m22, m32,
          0,   0,   0,   1,
    };

    // 每一列的向量长度
    auto l0 = std::hypot(m00, m01, m02);
    auto l1 = std::hypot(m10, m11, m12);
    auto l2 = std::hypot(m20, m21, m22);

    glm::mat4 rotateMat{
        m00/l0, m10/l1, m20/l2, 0,
        m01/l0, m11/l1, m21/l2, 0,
        m02/l0, m12/l1, m22/l2, 0,
             0,      0,      0, 1,
    };

    glm::mat4 translationMat{
        1, 0, 0, m30,
        0, 1, 0, m31,
        0, 0, 1, m32,
        0, 0, 0,   1,
    };

    glm::mat4 scaleMat{
        l0,  0,  0, 0,
         0, l1,  0, 0,
         0,  0, l2, 0,
         0,  0,  0, 1,
    };

    std::cout <<
        "-----------result mat:--------------\n" << resultMat <<
        "-----------rotate mat:--------------\n" << rotateMat <<
        "-----------scale mat:--------------\n" << scaleMat <<
        "-----------translation mat:--------------\n" << translationMat << '\n';

    // 只有这个resultMat和定义的相同
    resultMat = scaleMat * rotateMat * translationMat;
    std::cout << "-----------result mat:--------------\n" << resultMat;

    resultMat = translationMat * scaleMat * rotateMat;
    std::cout << "-----------result mat:--------------\n" << resultMat;

    resultMat = translationMat * rotateMat * scaleMat;
    std::cout << "-----------result mat:--------------\n" << resultMat;

    resultMat = rotateMat * scaleMat * translationMat;
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
    auto orthoMat = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);

    // 透视投影矩阵
    // 这将创建一个透视投影矩阵，其视角为45度，宽高比为800:600，近平面距离为0.1，远平面距离为100。
    // 这个矩阵可以用于渲染一个视锥体场景，其中近处物体看起来比远处物体更大。
    auto perspectiveMat = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

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
              << viewMat;

    return 0;
}

#endif // TEST6
