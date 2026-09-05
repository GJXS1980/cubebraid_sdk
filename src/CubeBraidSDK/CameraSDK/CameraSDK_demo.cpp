#include <iostream>
#include <iomanip>
#include "CubeBraidSDK/CameraSDK/CameraSDK.h" // 替换为你实际的头文件名称

int main()
{
    std::cout << "=== Camera3D C++ SDK Demo 启动 ===" << std::endl;
    camera3d_sdk::Camera3D camera;

    // 配置相机参数
    std::string camera_ip = "192.168.23.203"; 

    // 准备算法输入参数
    // 手眼标定参数示例
    camera3d_sdk::CalibrationPose calib_pose;
    calib_pose.x = 0.107419f;
    calib_pose.y = 1.03611f;
    calib_pose.z = 0.23078f;
    calib_pose.qw = -0.4595f;
    calib_pose.qx = 0.595359f;
    calib_pose.qy = -0.519468f;
    calib_pose.qz = 0.405645f;

    // 常用 AGV / 模式参数
    const float agv_x = 1.643f;                    // AGV 前方距离
    const float agv_y = 0.387f;                    // AGV 左侧距离
    const float angle = 0.3f;                     // 倾角仪角度
    const float j1_angle = 15.0f;                 // 一轴关节角
    const bool integrated_mode = false;          // false: 摆台模式, true: 装卸一体模式

    // ------------------------------------------------------------
    // 调用算法接口 1: 集装箱内部/斜坡基准点计算 (processTradition)
    // ------------------------------------------------------------
    std::cout << "\n--- 执行：集装箱内/斜坡基准点计算 ---" << std::endl;
    camera3d_sdk::Point3D tradition_result;
    camera.processTradition(
        calib_pose,
        camera_ip,
        0,                 // model_mod: 0-第一面顶吸基准点, 1-其它面
        agv_x,
        agv_y,
        angle,
        j1_angle,
        integrated_mode,
        tradition_result
    );

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "[SUCCESS] 传统基准点计算结果 -> X: " << tradition_result.x 
                << " m, Y: " << tradition_result.y 
                << " m, Z: " << tradition_result.z << " m" << std::endl;

    // ------------------------------------------------------------
    // 调用算法接口 2: 最后一面侧吸基准点计算 (processLastSurface)
    // ------------------------------------------------------------
    std::cout << "\n--- 执行：最后一面侧吸基准点计算 ---" << std::endl;
    camera3d_sdk::Point3D last_surface_result;
    camera.processLastSurface(
        calib_pose,
        camera_ip,
        agv_x,
        agv_y,
        j1_angle,
        integrated_mode,
        last_surface_result
    );

    std::cout << "[SUCCESS] 最后一面基准点计算结果 -> X: " << last_surface_result.x 
        << " m, Y: " << last_surface_result.y 
        << " m, Z: " << last_surface_result.z << " m" << std::endl;

    // ------------------------------------------------------------
    // 调用算法接口 3: 航向角偏差计算 (processYaw)
    // ------------------------------------------------------------
    // 配置相机参数
    std::string camera_ip_up = "192.168.23.203"; 

    // 手眼标定参数示例
    camera3d_sdk::CalibrationPose calib_pose_up;
    calib_pose_up.x = 0.107419f;
    calib_pose_up.y = 1.03611f;
    calib_pose_up.z = 0.23078f;
    calib_pose_up.qw = -0.4595f;
    calib_pose_up.qx = 0.595359f;
    calib_pose_up.qy = -0.519468f;
    calib_pose_up.qz = 0.405645f;

    std::cout << "\n--- 执行：AGV 航向角偏差计算 ---" << std::endl;
    double yaw_bias = 0.0;
    camera.processYaw(
        calib_pose_up,
        camera_ip_up,
        agv_x,
        agv_y,
        j1_angle,
        integrated_mode,
        yaw_bias
    );

    std::cout << std::setprecision(4);
    std::cout << "[SUCCESS] 偏航角偏差 (Yaw): " << yaw_bias << " 度" << std::endl;

    return 0;
}