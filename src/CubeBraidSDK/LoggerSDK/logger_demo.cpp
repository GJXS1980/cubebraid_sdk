#include "CubeBraidSDK/LoggerSDK/LoggerSDK.h"
#include <vector>
#include <iostream>

int main() 
{
    try 
    {
        // 获取默认单例并初始化
        auto& logger = logger_sdk::SystemLogger::instance();
        logger.init("log/logger_demo.log");

        // 普通文本日志输出
        logger.log("C++ SDK 初始化成功，开始执行任务...");

        // 模板格式化日志输出（支持基础数据类型）
        int step = 1;
        double process = 85.5;
        logger.log("当前进度 [步骤 %]: % %", step, process, "%");

        // 模板格式化日志输出（支持容器 vector 类型直接展开）
        std::vector<int> sensor_ids = { 101, 102, 105, 208 };
        logger.log("检测到在线传感器列表: %", sensor_ids);

        // 触发紧急停止接口
        logger.emergencyStop("机械臂到达极限位置，触发软限位！");

        // 支持创建独立的日志实例（例如单独记录网络日志）
        auto& netLogger = logger_sdk::SystemLogger::instance("netlogger");
        netLogger.init("log/network_demo.log");
        netLogger.log("网络模块连接就绪，监听端口: 8080");

    } 
    catch (const std::exception& e) 
    {
        std::cerr << "发生异常: " << e.what() << std::endl;
    }

    return 0;
}