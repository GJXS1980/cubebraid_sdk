#include <iostream>
#include <iomanip>
#include <vector>
#include "CubeBraidSDK/CameraSDK/CameraSDK.h"

using namespace camera3d_sdk;

// 辅助打印错误函数
void printError(Camera3D& camera, StatusCode status, const std::string& action) {
    std::cerr << "[FAIL] " << action << " 失败!" << std::endl;
    std::cerr << "  - 错误码: " << static_cast<int>(status) 
              << " (" << statusToString(status) << ")" << std::endl;
    std::cerr << "  - 详细信息: " << camera.getLastError() << std::endl;
}

int main() 
{
    std::cout << "================ C++ Camera3D SDK Demo ================\n";
    Camera3D camera;

    // 结构体初始化配置
    CameraConfig config;
    config.camera_ip = "192.168.23.203";
    config.camera_ip_up = "192.168.23.88";
    config.depth_file = "./data/img/depth.tiff";
    config.color_file = "./data/img/demo.png";
    config.corner_model_path = "./data/models/corner_model.onnx";
    config.box_model_path = "./data/models/box_model.onnx";

    StatusCode status = camera.initialize(config);
    if (status != StatusCode::SUCCESS) {
        printError(camera, status, "初始化 SDK");
        return -1;
    }
    std::cout << "[SUCCESS] SDK 初始化完成." << std::endl;

    // 2. 建立连接
    if (!camera.connect()) {
        std::cerr << "[FAIL] 相机连接失败: " << camera.getLastError() << std::endl;
        return -1;
    }
    std::cout << "[SUCCESS] 相机连接成功, 状态: " << (camera.isConnected() ? "在线" : "离线") << std::endl;

    // 3. 参数配置
    camera.setCameraMode(1);
    camera.setDistance(1.5);
    camera.setDepthDistance(2.0);

    std::cout << "[SUCCESS] 参数初始化完成." << std::endl;

    // 机器人外参
    double robot_matrix[16] = 
    {
        1.0, 0.0, 0.0, 0.2,
        0.0, 1.0, 0.0, 0.1,
        0.0, 0.0, 1.0, 0.5,
        0.0, 0.0, 0.0, 1.0
    };
    status = camera.setRobotTransform(robot_matrix);
    std::cout << "[SUCCESS] 相机到机器人变换完成." << std::endl;
    if (status != StatusCode::SUCCESS) 
    {
        printError(camera, status, "设置变换矩阵");
    }

    // 4. 算法业务调用
    Point3D result_pt;
    std::cout << "[SUCCESS] 开始普通基准点识别." << std::endl;
    
    // 4.1 普通基准点算法
    status = camera.processTradition(1, 0, 10.5f, 5.2f, result_pt);

    std::cout << "[SUCCESS] 普通基准点识别完成." << std::endl;
    if (status == StatusCode::SUCCESS) 
    {
        std::cout << "-> [Tradition] 结果: X=" << result_pt.x << ", Y=" << result_pt.y << ", Z=" << result_pt.z << std::endl;
    } 
    else 
    {
        printError(camera, status, "执行 processTradition");
    }

    // 4.2 坡度场景算法
    status = camera.processSlope(1, 0, 10.5f, 5.2f, 15.0f, result_pt);
    if (status == StatusCode::SUCCESS) 
    {
        std::cout << "-> [Slope] 结果: X=" << result_pt.x << ", Y=" << result_pt.y << ", Z=" << result_pt.z << std::endl;
    } 
    else 
    {
        printError(camera, status, "执行 processSlope");
    }

    // 4.3 Yaw 角计算
    double yaw_angle = 0.0;
    status = camera.processYaw(1, 12.3f, 4.5f, yaw_angle);
    if (status == StatusCode::SUCCESS) 
    {
        std::cout << "-> [Yaw] 计算结果: " << yaw_angle << " rad" << std::endl;
    } 
    else 
    {
        printError(camera, status, "执行 processYaw");
    }

    // 5. 断开资源
    camera.disconnect();
    std::cout << "[SUCCESS] 相机断开连接.\n";

    return 0;
}