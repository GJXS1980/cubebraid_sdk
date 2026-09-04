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
    std::cout << "[AGV Demo] 发送前进到底指令: 1800mm (超时 100s)..." << std::endl;
    if (agv.goForward(1800.0, 100000)) 
    {
        std::cout << "[AGV Demo] 前进到底到位！" << std::endl;
    } 
    else 
    {
        std::cerr << "[AGV Demo] 前进到底指令超时或执行失败！" << std::endl;
    }

    // 更新位姿
    pose = agv.getPose();
    std::cout << "前进后位姿 -> 前方距离 X: " << pose.roll << "m" << std::endl;
    std::cout << "前进后位姿 -> 左侧距离 Y: " << pose.pitch << "m" << std::endl;

    // 前进 500mm
    std::cout << "[AGV Demo] 发送单步前进指令: 500mm (超时 100s)..." << std::endl;
    if (agv.goBack(500.0, 100000)) 
    {
        std::cout << "[AGV Demo] 单步前进到位！" << std::endl;
    }

    // 更新位姿
    pose = agv.getPose();
    std::cout << "前进后位姿 -> 前方距离 X: " << pose.roll << "m" << std::endl;
    std::cout << "前进后位姿 -> 左侧距离 Y: " << pose.pitch << "m" << std::endl;

    // 后退 500mm
    std::cout << "[AGV Demo] 发送单步后退指令: 500mm (超时 100s)..." << std::endl;
    if (agv.goBack(-500.0, 100000)) 
    {
        std::cout << "[AGV Demo] 单步后退到位！" << std::endl;
    }

    // 更新位姿
    pose = agv.getPose();
    std::cout << "前进后位姿 -> 前方距离 X: " << pose.roll << "m" << std::endl;
    std::cout << "前进后位姿 -> 左侧距离 Y: " << pose.pitch << "m" << std::endl;

    // 切换为手动控制模式
    std::cout << "[AGV Demo] 切换为手动控制模式..." << std::endl;
    agv.switchControlMode(agv_sdk::ControlMode::Manual);

    std::cout << "[AGV Demo] 切换为手动速度控制模式: 100mm/s速度前进2000ms (自动重发与归零停止)..." << std::endl;
    // 传入 vx=100.0, vy=0.0, w=0.0，持续时间 2000ms，内部每 50ms 重发一次
    agv.moveManualForDuration(100.0f, 0.0f, 0.0f, 2000, 50);

    std::cout << "[AGV Demo] 切换为手动速度控制模式: -100mm/s速度前进2000ms (自动重发与归零停止)..." << std::endl;
    // 传入 vx=-100.0, vy=0.0, w=0.0，持续时间 2000ms，内部每 50ms 重发一次
    agv.moveManualForDuration(-100.0f, 0.0f, 0.0f, 2000, 50);

    std::cout << "[AGV Demo] 切换为手动速度控制模式: 0.1rad/s速度前进2000ms (自动重发与归零停止)..." << std::endl;
    // 传入 vx=0.0, vy=0.0, w=100.0，持续时间 2000ms，内部每 50ms 重发一次
    agv.moveManualForDuration(0.0f, 0.0f, 100.0f, 2000, 50);

    std::cout << "[AGV Demo] 切换为手动速度控制模式: -0.1rad/s速度前进2000ms (自动重发与归零停止)..." << std::endl;
    // 传入 vx=0.0, vy=0.0, w=-100.0，持续时间 2000ms，内部每 50ms 重发一次
    agv.moveManualForDuration(0.0f, 0.0f, -100.0f, 2000, 50);

    std::cout << "[AGV Demo] 手动控制结束并已停止" << std::endl;

    // 登出与断开
    std::cout << "[AGV Demo] 断开连接..." << std::endl;
    agv.logout();
    agv.disconnectAGV();

    std::cout << "[AGV Demo] 运行结束。" << std::endl;
    return 0;
}