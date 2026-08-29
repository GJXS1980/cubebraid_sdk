#include <iostream>
#include <thread>
#include <chrono>
#include "CubeBraidSDK/AGV_SDK/AGV_SDK.h"

int main() 
{
    std::cout << "[AGV Demo] 初始化 AGV 控制器..." << std::endl;
    agv_sdk::AGVController agv;

    // 连接设备
    const std::string target_ip = "192.168.1.91"; // AGV IP
    const int target_port = 5005;   // AGV 端口
    
    std::cout << "[AGV Demo] 正在连接设备: " << target_ip << ":" << target_port << "..." << std::endl;
    if (!agv.connectAGV(target_ip, target_port)) 
    {
        std::cerr << "[AGV Demo] 连接失败，请检查网络设置。" << std::endl;
        return -1;
    }
    std::cout << "[AGV Demo] 连接成功！" << std::endl;

    // 登录认证 (假设密码已为 Hash 值)
    std::cout << "[AGV Demo] 发起登录请求..." << std::endl;
    if (!agv.login("admin", "21232f297a57a5a743894a0e4a801fc3")) 
    {
        std::cerr << "[AGV Demo] 登录失败！" << std::endl;
        agv.disconnectAGV();
        return -1;
    }
    std::cout << "[AGV Demo] 登录成功！" << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(200));    // 200ms

    // 切换为自动控制模式
    std::cout << "[AGV Demo] 切换为自动控制模式..." << std::endl;
    agv.switchControlMode(agv_sdk::ControlMode::Auto);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));    // 200ms

    // 查询当前状态与位姿
    agv.querySystemState();
    agv_sdk::AGVPose pose = agv.getPose();
    std::cout << "前进后位姿 -> 前方距离 X: " << pose.roll << "m" << std::endl;
    std::cout << "前进后位姿 -> 左侧距离 Y: " << pose.pitch << "m" << std::endl;
    // 控制移动：前进 1800mm
    std::cout << "[AGV Demo] 发送前进指令: 1800mm (超时 100s)..." << std::endl;
    if (agv.goForward(1800.0, 100000)) 
    {
        std::cout << "[AGV Demo] 前进到位！" << std::endl;
    } 
    else 
    {
        std::cerr << "[AGV Demo] 前进指令超时或执行失败！" << std::endl;
    }

    // 更新位姿
    pose = agv.getPose();
    std::cout << "前进后位姿 -> 前方距离 X: " << pose.roll << "m" << std::endl;
    std::cout << "前进后位姿 -> 左侧距离 Y: " << pose.pitch << "m" << std::endl;

    // 后退 500mm
    std::cout << "[AGV Demo] 发送后退指令: 500mm (超时 100s)..." << std::endl;
    if (agv.goBack(-500.0, 100000)) 
    {
        std::cout << "[AGV Demo] 后退到位！" << std::endl;
    }

    // 更新位姿
    pose = agv.getPose();
    std::cout << "前进后位姿 -> 前方距离 X: " << pose.roll << "m" << std::endl;
    std::cout << "前进后位姿 -> 左侧距离 Y: " << pose.pitch << "m" << std::endl;

    // 切换为手动控制模式
    std::cout << "[AGV Demo] 切换为手动控制模式..." << std::endl;
    agv.switchControlMode(agv_sdk::ControlMode::Manual);

    // 切换为手动控制模式
    std::cout << "[AGV Demo] 切换为手动速度控制模式: 100mm/s速度前进200ms..." << std::endl;
    agv.manualCtlVelSet(1000, 0.0, 0.0);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));    // 200ms

    std::cout << "[AGV Demo] 停止手动速度控制模式" << std::endl;
    agv.manualCtlVelSet(0, 0.0, 0.0);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));    // 100ms

    // 登出与断开
    std::cout << "[AGV Demo] 断开连接..." << std::endl;
    agv.logout();
    agv.disconnectAGV();

    std::cout << "[AGV Demo] 运行结束。" << std::endl;
    return 0;
}