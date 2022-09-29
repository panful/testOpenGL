
/*
* 1. 向量的 加法 叉乘 点乘 矩阵
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
