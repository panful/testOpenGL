
/*
* 1. 向量的 加法 叉乘 点乘 矩阵
* 2. games101
*/

#define TEST1

#ifdef TEST1

#include <iostream>
#include <Eigen/Core>
#include <Eigen/Dense>

int main()
{
    Eigen::Vector3f v1(1.f, 2.f, 3.f);
    Eigen::Vector3f v2(4.f, 5.f, 6.f);
    std::cout << "Eigen::Vector3f(1.f, 2.f, 3.f) = \n" << v1 << std::endl << std::endl;
    std::cout << "Eigen::Vector3f(4.f, 5.f, 6.f) = \n" << v2 << std::endl << std::endl;

    std::cout << "----------------\n";

    // 向量和标量运算，Eigen中向量不能使用 + - 运算符
    std::cout << "(1.f, 2.f, 3.f) * 3 = \n" << v1 * 3 << std::endl << std::endl;
    std::cout << "(1.f, 2.f, 3.f) / 3 = \n" << v1 / 3 << std::endl << std::endl;

    std::cout << "----------------\n";

    // 相加
    std::cout << "(1.f, 2.f, 3.f) + (4.f, 5.f, 6.f) = \n" << v1 + v2 << std::endl << std::endl;
    // 点积
    std::cout << "(1.f, 2.f, 3.f) . (4.f, 5.f, 6.f) = \n" << v1.dot(v2) << std::endl << std::endl;
    // 叉积
    std::cout << "(1.f, 2.f, 3.f) x (4.f, 5.f, 6.f) = \n" << v1.cross(v2) << std::endl << std::endl;

    std::cout << "----------------\n";

    Eigen::Vector2f v3(1.f, 2.f);
    Eigen::Vector2f v4(2.f, 3.f);
    std::cout << "(1.f, 2.f) . (2.f, 3.f) = \n" << v3.dot(v4) << std::endl << std::endl;;
    // v3.cross(v4); // 2维向量不能做叉乘

    std::cout << "----------------\n";

    Eigen::Matrix3f m1;
    Eigen::Matrix3f m2; // 不能用(){}构造
    m1 << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0;
    m2 << 2.0, 3.0, 1.0, 4.0, 6.0, 5.0, 9.0, 7.0, 8.0;

    std::cout << "m1 = \n" << m1 << std::endl << std::endl;
    std::cout << "m2 = \n" << m2 << std::endl << std::endl;
    std::cout << "m1 + m2 = \n" << m1 + m2 << std::endl << std::endl;
    std::cout << "2 * m1 = \n" << 2 * m1 << std::endl << std::endl;
    std::cout << "m1 * m2 = \n" << m1 * m2 << std::endl << std::endl;
    std::cout << "Eigen::Vector3f(1.f, 2.f, 3.f) * m1 = \n" << m1 * v1 << std::endl << std::endl; // 向量和矩阵做乘法，矩阵必须在前面

    // 单位矩阵（对角线上的元素为1）
    Eigen::Matrix4f m3 = Eigen::Matrix4f::Identity();
    std::cout << "Eigen::Matrix4f::Identity() = \n" << m3 << std::endl << std::endl;

    // 矩阵元素未知
    Eigen::Matrix4f m4;
    std::cout << "Eigen::Matrix4f = \n" << m4 << std::endl << std::endl;

}

#endif // TEST1

#ifdef TEST2

#include <cmath>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <iostream>

int main() {

    // Basic Example of cpp
    std::cout << "Example of cpp \n";
    float a = 1.0, b = 2.0;
    std::cout << a << std::endl;
    std::cout << a / b << std::endl;
    std::cout << std::sqrt(b) << std::endl;
    std::cout << std::acos(-1) << std::endl;
    std::cout << std::sin(30.0 / 180.0 * acos(-1)) << std::endl;

    // Example of vector
    std::cout << "Example of vector \n";
    // vector definition
    Eigen::Vector3f v(1.0f, 2.0f, 3.0f);
    Eigen::Vector3f w(1.0f, 0.0f, 0.0f);
    // vector output
    std::cout << "Example of output \n";
    std::cout << v << std::endl;
    // vector add
    std::cout << "Example of add \n";
    std::cout << v + w << std::endl;
    // vector scalar multiply
    std::cout << "Example of scalar multiply \n";
    std::cout << v * 3.0f << std::endl;
    std::cout << 2.0f * v << std::endl;

    // Example of matrix
    std::cout << "Example of matrix \n";
    // matrix definition
    Eigen::Matrix3f i, j;
    i << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0;
    j << 2.0, 3.0, 1.0, 4.0, 6.0, 5.0, 9.0, 7.0, 8.0;
    // matrix output
    std::cout << "Example of output \n";
    std::cout << i << std::endl;
    // matrix add i + j
    // matrix scalar multiply i * 2.0
    // matrix multiply i * j
    // matrix multiply vector i * v

    return 0;
}

#endif // TEST2
