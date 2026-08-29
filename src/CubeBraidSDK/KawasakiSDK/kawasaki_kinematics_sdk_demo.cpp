#include <iostream>
#include "CubeBraidSDK/KawasakiSDK/kawasaki_kinematics_sdk.h"

int main() 
{
    KawasakiRS080N::KinematicsSolver solver;

    // 正运动学测试
    std::vector<double> q_deg = {-144, -6.48, -96.01, -7.19, -87, 111.97}; // 六个关节角
    Eigen::Matrix4d T = solver.forward_kinematics(q_deg);
    std::cout << "--- C++ Demo ---" << std::endl;
    std::cout << "正运动学 T06 矩阵:\n" << T << "\n\n";

    // 逆运动学测试
    auto solutions = solver.inverse_kinematics(T);
    std::cout << "逆运动学计算求解个数: " << solutions.size() << std::endl;
    for (size_t i = 0; i < solutions.size(); ++i) 
    {
        std::cout << "解 " << i + 1 << ": ";
        for (double val : solutions[i]) std::cout << val << " ";
        std::cout << "\n";
    }

    return 0;
}