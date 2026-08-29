#include "CubeBraidSDK/RobotSDK/RobotSDK.h"
#include <iostream>
#include <iomanip>

#include <thread>
#include <chrono>

using namespace robot_sdk;

int main() 
{
    std::cout << "========== 机器人 SDK C++ 测试程序 ==========\n" << std::endl;

    // 实例化 SDK 主对象
    RobotController robot;

    // 机械臂连接测试
    std::string robotIP = "192.168.0.2"; // 替换为实际机械臂 IP
    std::cout << "[通信测试] 正在连接机器人: " << robotIP << "..." << std::endl;

    if (!robot.connectRobot(robotIP, 31400, 31401)) {
        std::cerr << "[通信测试] 连接失败！(提示: 如果仅测试算法，可忽略连接失败)\n" << std::endl;
    } 
    else 
    {
        std::cout << "[通信测试] 连接成功！\n" << std::endl;

        // 关节空间运动控制测试 (前往粗略过渡点)
        std::cout << "[运动控制] 发送关节运动指令 (Coarse Mode)..." << std::endl;
        Joint targetJoint{-85.0, -13.73, -96.28, 0.0, -97.45, 0.0};
        bool joint_cont_flag = robot.controlJoint(ControlMode::Coarse, targetJoint);
        while (joint_cont_flag == false) 
        {
            std::cout << "[运动控制] 关节运动响应失败,重新发送！" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            joint_cont_flag = robot.controlJoint(ControlMode::Coarse, targetJoint);
        } 
        std::cout << "[运动控制] 关节运动响应成功！" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // 笛卡尔空间位姿控制测试 (前往精准目标点)
        std::cout << "[运动控制] 发送位姿运动指令 (Fine Mode)..." << std::endl;
        Pose targetPose{-0.03, 1014.32, 1500.16, -82.69, 180.0, 7.31};
        bool cont_flag = robot.controlPosture(ControlMode::Fine, targetPose);
        
        while (cont_flag == false)
        {
            std::cout << "[运动控制] 位姿运动响应失败,重新发送！" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            cont_flag = robot.controlPosture(ControlMode::Fine, targetPose);
        } 

        std::cout << "[运动控制] 位姿运动成功接收！" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // 断开连接
        robot.disconnectRobot();
        std::cout << "[通信测试] 已断开机器人连接。\n" << std::endl;
    }

    // 抓取与斜面补偿算法测试 (无需连接物理实体即可测试)
    std::cout << "========== 抓取补偿算法测试 ==========" << std::endl;

    // 假设输入参数：质心坐标、SKU 尺寸、相机偏移等
    Pose centroid{0.45, -0.12, 0.30}; // 米 (算法内部自动换算成 mm)
    BoxDimension box{400.0f, 300.0f, 200.0f}; // mm
    Pose poseOffset{5.0, -10.0, 0.0, 0.0, 0.0, 0.0}; // mm
    
    int fetchMode = 1;      // 吸取长边
    int skuNum = 2;         // 2 箱抓取
    float disY = 15.0f;     // 缝隙补偿
    bool isROffset = true;  // 左侧计算
    int modelMod = 0;
    double inclxAngle = 3.5; // 倾角 3.5°

    // 调用 SDK 算位姿
    Pose resultPose = RobotController::Top_suction_angle(
        centroid, box, fetchMode, skuNum, disY, 
        poseOffset, isROffset, modelMod, inclxAngle
    );

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "输入参数:\n"
              << "  - 箱体尺寸: " << box.length << " x " << box.width << " x " << box.height << " mm\n"
              << "  - 倾角: " << inclxAngle << "°\n"
              << "计算得出的目标机械臂位姿 (mm):\n"
              << "  - Target X: " << resultPose.x << " mm\n"
              << "  - Target Y: " << resultPose.y << " mm\n"
              << "  - Target Z: " << resultPose.z << " mm\n"
              << "========================================\n";

    return 0;
}