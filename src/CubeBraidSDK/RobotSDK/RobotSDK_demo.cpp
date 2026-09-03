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
    double initJ4 = 0.0;

    // 机械臂连接测试
    std::string robotIP = "192.168.0.2"; // 替换为实际机械臂 IP
    std::cout << "[通信测试] 正在连接机器人: " << robotIP << "..." << std::endl;

    if (!robot.connectRobot(robotIP, 31400, 31401)) 
    {
        std::cerr << "[通信测试] 连接失败！\n" << std::endl;
    } 
    else 
    {
        std::cout << "[通信测试] 连接成功！\n" << std::endl;

        // 设置浮点数输出格式
        std::cout << std::fixed << std::setprecision(2);

        std::this_thread::sleep_for(std::chrono::seconds(2));

        // ============================================================
        // 获取初始状态（运动前的位姿、关节角、4轴角度）
        // ============================================================
        Pose initPose = robot.GetCurrentPose();
        Joint initJoint = robot.GetCurrentJoint();
        initJ4 = robot.GetJoint4Angle();

        std::cout << "\n[状态查询] 初始位姿 (Cartesian): "
                  << "X: " << initPose.x << " mm, "
                  << "Y: " << initPose.y << " mm, "
                  << "Z: " << initPose.z << " mm, "
                  << "RX: " << initPose.rx << "°, "
                  << "RY: " << initPose.ry << "°, "
                  << "RZ: " << initPose.rz << "°" << std::endl;

        std::cout << "[状态查询] 初始关节角 (Joints): "
                  << "J1: " << initJoint.j1 << "°, "
                  << "J2: " << initJoint.j2 << "°, "
                  << "J3: " << initJoint.j3 << "°, "
                  << "J4: " << initJoint.j4 << "°, "
                  << "J5: " << initJoint.j5 << "°, "
                  << "J6: " << initJoint.j6 << "°" << std::endl;
        std::cout << "[状态查询] J4 轴独立角度: " << initJ4 << "°\n" << std::endl;

        // ============================================================
        // 关节空间运动控制测试 (前往粗略过渡点)
        // ============================================================
        std::cout << "[运动控制] 发送关节运动指令 (Coarse Mode)..." << std::endl;
        Joint targetJoint{-90.0, -13.73, -96.28, 20.0, -97.45, 0.0};
        bool joint_cont_flag = robot.controlJoint(ControlMode::Coarse, targetJoint);
        while (joint_cont_flag == false) 
        {
            std::cout << "[运动控制] 关节运动响应失败,重新发送！" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            joint_cont_flag = robot.controlJoint(ControlMode::Coarse, targetJoint);
        } 
        std::cout << "[运动控制] 关节运动响应成功！" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));

        // 关节运动后的当前状态获取
        Joint currentJoint = robot.GetCurrentJoint();
        initJ4 = robot.GetJoint4Angle();
        std::cout << "[状态更新] 关节运动后实际关节角: "
                  << "J1: " << currentJoint.j1 << "°, "
                  << "J2: " << currentJoint.j2 << "°, "
                  << "J3: " << currentJoint.j3 << "°, "
                  << "J4: " << currentJoint.j4 << "°, "
                  << "J5: " << currentJoint.j5 << "°, "
                  << "J6: " << currentJoint.j6 << "°\n" << std::endl;

        std::cout << "[状态查询] J4 轴独立角度: " << initJ4 << "°\n" << std::endl;

        // ============================================================
        // 笛卡尔空间位姿控制测试 (前往精准目标点)
        // ============================================================
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
        std::this_thread::sleep_for(std::chrono::seconds(5));

        // 位姿运动后的当前状态获取
        Pose currentPose = robot.GetCurrentPose();
        initJ4 = robot.GetJoint4Angle();

        std::cout << "[状态更新] 位姿运动后实际位姿: "
                  << "X: " << currentPose.x << " mm, "
                  << "Y: " << currentPose.y << " mm, "
                  << "Z: " << currentPose.z << " mm, "
                  << "RX: " << currentPose.rx << "°, "
                  << "RY: " << currentPose.ry << "°, "
                  << "RZ: " << currentPose.rz << "°\n" << std::endl;
        std::cout << "[状态查询] J4 轴独立角度: " << initJ4 << "°\n" << std::endl;

        // 断开连接
        robot.disconnectRobot();
        std::cout << "[通信测试] 已断开机器人连接。\n" << std::endl;
    }

    // 抓取与斜面补偿算法测试 (无需连接物理实体即可测试)
    std::cout << "========== 抓取补偿算法测试 ==========" << std::endl;

    // 假设输入参数：质心坐标、SKU 尺寸、相机偏移等
    Pose centroid{1.46328, 1.52448, -0.611929}; // 基准点，单位米 (算法内部自动换算成 mm)
    BoxDimension box{570.0f, 453.0f, 330.0f}; // 产品尺寸,单位mm
    Pose poseOffset{-570.0, -453.0, 330.0, 0.0, 0.0, 0.0}; // 垛型x,y,z偏移量,单位mm
    
    int fetchMode = 3;      // 抓取模式 (1,2: 沿长边抓取; 3,4: 沿短边抓取)
    int skuNum = 1;         // 并排抓取的 SKU 数量
    float disY = 75.0f;     // 每层剩余缝隙
    bool isROffset = true;  // 垛型最左侧标志位 (true: 最左侧, false: 右侧)
    int modelMod = 0;       // 是否为第一面(0为第一面, 1为其它面)
    double inclxAngle = 0; // 倾角 0°

    // 顶吸偏移量xyz求解
    Pose resultPose = RobotController::Top_suction_angle(
        centroid, box, fetchMode, skuNum, disY, 
        poseOffset, isROffset, modelMod, inclxAngle
    );

    std::cout << "输入参数:\n"
              << "  - 箱体尺寸: " << box.length << " x " << box.width << " x " << box.height << " mm\n"
              << "  - 倾角: " << inclxAngle << "°\n"
              << "计算得出的目标机械臂位姿 (mm):\n"
              << "  - Target X: " << resultPose.x << " mm\n"
              << "  - Target Y: " << resultPose.y << " mm\n"
              << "  - Target Z: " << resultPose.z << " mm\n"
              << "========================================\n";
    
    // 顶吸特殊码法偏移量xyz求解
    resultPose = RobotController::Top_suction_special(
        centroid, box, fetchMode, skuNum, disY, 
        poseOffset, isROffset, modelMod, inclxAngle
    );

    std::cout << "输入参数:\n"
              << "  - 箱体尺寸: " << box.length << " x " << box.width << " x " << box.height << " mm\n"
              << "  - 倾角: " << inclxAngle << "°\n"
              << "计算得出的目标机械臂位姿 (mm):\n"
              << "  - Target X: " << resultPose.x << " mm\n"
              << "  - Target Y: " << resultPose.y << " mm\n"
              << "  - Target Z: " << resultPose.z << " mm\n"
              << "========================================\n";

    int fetchMode_side = 3;      // 抓取模式 (1,2: 沿长边抓取; 3,4: 沿短边抓取)
    float container_h = 2698;      // 集装箱高度
    bool switch_top_bottom_suction = false;  // 上层/下次侧吸切换标志位 (true: 下层侧吸(酒类), false: 上层侧吸(默认))
    // 上层侧吸偏移量xyz求解
    resultPose = RobotController::Side_suction_angle(
        centroid, box, fetchMode, fetchMode_side, skuNum, disY, 
        poseOffset, isROffset, modelMod, inclxAngle, container_h, switch_top_bottom_suction
    );

    std::cout << "输入参数:\n"
              << "  - 箱体尺寸: " << box.length << " x " << box.width << " x " << box.height << " mm\n"
              << "  - 倾角: " << inclxAngle << "°\n"
              << "计算得出的目标机械臂位姿 (mm):\n"
              << "  - Target X: " << resultPose.x << " mm\n"
              << "  - Target Y: " << resultPose.y << " mm\n"
              << "  - Target Z: " << resultPose.z << " mm\n"
              << "========================================\n";

    // 下层侧吸偏移量xyz求解
    switch_top_bottom_suction = true;
    resultPose = RobotController::Side_suction_angle(
        centroid, box, fetchMode, fetchMode_side, skuNum, disY, 
        poseOffset, isROffset, modelMod, inclxAngle, container_h, switch_top_bottom_suction
    );

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