#include "../Polynomial.h"
#include <iostream>
#include <vector>

void testPolynomial()
{
    // 测试构造函数
    std::cout << "---- Testing Constructors ----" << std::endl;
    Polynomial p1; // 默认构造函数
    Polynomial p2({1.0, 2.0, 3.0}); // 初始化列表构造
    Polynomial p3(std::vector<double>{4.0, 5.0, 6.0}); // 使用向量构造
    Polynomial p4(p2); // 复制构造
    double arr[] = {7.0, 8.0, 9.0};
    Polynomial p5(arr, 3); // 使用数组构造

    std::cout << "p2: " << p2 << std::endl;
    std::cout << "p3: " << p3 << std::endl;
    std::cout << "p4: " << p4 << std::endl;
    std::cout << "p5: " << p5 << std::endl;

    // 测试系数访问与修改
    std::cout << "\n---- Testing Coefficient Access ----" << std::endl;
    std::cout << "p3[0]: " << p3[0] << ", p3[1]: " << p3[1] << ", p3[2]: " << p3[2] << std::endl;
    p3[0] = 10.0;
    std::cout << "Modified p3: " << p3 << std::endl;

    // 测试计算多项式值
    std::cout << "\n---- Testing Polynomial Evaluation ----" << std::endl;
    std::cout << "p2(2.0): " << p2(2.0) << std::endl;

    // 测试求导
    std::cout << "\n---- Testing Derivatives ----" << std::endl;
    Polynomial p6({1.0, -1.0, 1.0, -1.0}); // p6 = 1 - x + x^2 - x^3
    std::cout << "p6: " << p6 << std::endl;
    std::cout << "p6'(x): " << p6.Diff() << std::endl;
    std::cout << "p6''(x): " << p6.Diff(2) << std::endl;

    // 测试多项式加法
    std::cout << "\n---- Testing Addition ----" << std::endl;
    Polynomial p7({2.0, 3.0}); // p7 = 2 + 3x
    Polynomial p8({1.0, 2.0, 4.0}); // p8 = 1 + 2x + 4x^2
    Polynomial pAdd = p7 + p8;
    std::cout << "p7: " << p7 << std::endl;
    std::cout << "p8: " << p8 << std::endl;
    std::cout << "p7 + p8: " << pAdd << std::endl;

    // 测试多项式减法
    std::cout << "\n---- Testing Subtraction ----" << std::endl;
    Polynomial pSub = p8 - p7;
    std::cout << "p8 - p7: " << pSub << std::endl;

    // 测试多项式乘法
    std::cout << "\n---- Testing Multiplication ----" << std::endl;
    Polynomial pMul = p7 * p8;
    std::cout << "p7 * p8: " << pMul << std::endl;

    // 测试多项式除法
    std::cout << "\n---- Testing Division ----" << std::endl;
    Polynomial pDiv = p8 / 2.0;
    std::cout << "p8 / 2.0: " << pDiv << std::endl;

    // 测试多项式幂
    std::cout << "\n---- Testing Power ----" << std::endl;
    Polynomial pPow = p7.power(3);
    std::cout << "p7^3: " << pPow << std::endl;

    // 测试友元运算符
    std::cout << "\n---- Testing Friend Operators ----" << std::endl;
    Polynomial pFriendAdd = 2.0 + p8;
    Polynomial pFriendSub = 10.0 - p7;
    Polynomial pFriendMul = 3.0 * p7;
    std::cout << "2.0 + p8: " << pFriendAdd << std::endl;
    std::cout << "10.0 - p7: " << pFriendSub << std::endl;
    std::cout << "3.0 * p7: " << pFriendMul << std::endl;
}

int main()
{
    testPolynomial();
    return 0;
}
