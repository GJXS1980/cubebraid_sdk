#include "CubeBraidSDK/PLC_SDK/PLC_SDK.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() 
{
    // 实例化控制器
    plc_sdk::PLCController controller;

    // 1. 连接 PLC (IP, rack, slot)
    std::cout << "正在连接 PLC..." << std::endl;
    if (!controller.connect("192.168.30.49", 0, 1)) 
    {
        std::cerr << "连接 PLC 失败！" << std::endl;
        return -1;
    }
    std::cout << "PLC 连接成功！" << std::endl;

    // 触发 PLC 初始化脉冲
    std::cout << "正在触发 PLC 初始化..." << std::endl;
    if (controller.triggerPLCInit()) 
    {
        std::cout << "初始化指令已发送，等待 PLC 完成初始化..." << std::endl;
    }

    // 循环等待 PLC 初始化完成信号 (plc_init_state == 1)
    int timeout_sec = 10;
    while (timeout_sec > 0) 
    {
        PLCStatus status = controller.getStatus();
        if (status.plc_init_state == 1) 
        {
            std::cout << "PLC 初始化成功！" << std::endl;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        timeout_sec--;
    }

    if (timeout_sec <= 0) 
    {
        std::cerr << "警告: PLC 初始化响应超时！" << std::endl;
    }

    // std::this_thread::sleep_for(std::chrono::milliseconds(10000));

    // 发送取料参数数据 
    PickUpData pickup_data;
    pickup_data.fetch_mode = 4;     // 取料模式 (1/2 -> 理料模式4)
    pickup_data.mode_switch = 2;
    pickup_data.sku_l = 600;        // 长度 (mm)
    pickup_data.sku_w = 500;        // 宽度 (mm)
    pickup_data.sku_h = 400;        // 高度 (mm)
    pickup_data.sku_num = 1;        // 数量
    pickup_data.sku_weight = 5.5;   // 单个重量 (kg)

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    if (controller.sendPickUpData(pickup_data)) 
    {
        std::cout << "取料数据发送成功！" << std::endl;
    } 
    else 
    {
        std::cerr << "取料数据发送失败！" << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // 触发信号与参数更新
    controller.setOpenSuctionCup(true);                // 开启吸盘离散信号
    controller.setFixtureRollOut(true);                // 开启底托信号
    controller.setInclinometerErrorSignal(true);       
    controller.setSuctionCupPickingOK(true);       
    controller.setRetrievingCompletionSide(true);  

    controller.controlTableAngle(1);                   // 摆台角度模式
    controller.updateBoxState(6, 9, 4);               // 码垛箱数状态：面1，层2，箱5
    controller.updateInclinometerAngle(-3.5f);        // 更新倾角仪角度 12.5°
    controller.updateTotalBoxState(1000, 500);           // 总箱数 100，剩余 80

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // 4. 主线程循环获取后台线程读取到的实时 PLC 状态
    for (int i = 0; i < 2; ++i) 
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        
        PLCStatus status = controller.getStatus();
        std::cout << "--- 实时状态监控 [" << i + 1 << "] ---" << std::endl;
        std::cout << "吸盘状态: " << status.suction_cup_state << std::endl;
        std::cout << "夹具/底托状态: " << status.fixture_state << std::endl;
        std::cout << "机器人取料状态: " << status.robot_pick_state << std::endl;
        std::cout << "机器人离开状态: " << status.robot_leave_state << std::endl;

        std::cout << "摆台控制状态: " << status.table_control_state << std::endl;
        std::cout << "液压杆上升状态: " << status.hydraulic_rod_rise_state << std::endl;
        std::cout << "液压杆下降状态: " << status.hydraulic_rod_lower_state << std::endl;
        std::cout << "PLC初始化状态: " << status.plc_init_state << std::endl;
        std::cout << "摆台初始化完成状态: " << status.table_init_control_ok_state << std::endl;
        
        std::cout << "PLC请求数据状态: " << status.plc_data_request_state << std::endl;
    }

    // 关闭吸盘
    controller.setCloseSuctionCup(true);   
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    controller.setFixtureInitialization(true);     
    std::this_thread::sleep_for(std::chrono::milliseconds(200));              

    // 5. 断开连接并回收后台线程
    std::cout << "断开 PLC 连接..." << std::endl;
    controller.disconnect();

    return 0;
}