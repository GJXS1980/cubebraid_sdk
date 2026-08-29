#include <iostream>
#include <thread>
#include <chrono>

#include "CubeBraidSDK/SensorSDK/InclinometerSDK.h"


int main()
{
    std::cout << "Inclinometer SDK Demo" << std::endl;

    //==========================================================
    // 创建
    //==========================================================
    InclinometerSDK::Inclinometer sensor;

    //==========================================================
    // 配置
    //==========================================================
    InclinometerSDK::InclinometerConfig config;

    config.port = "\\\\.\\COM9";
    config.baudRate = 9600;
    config.autoReconnect = true;
    config.reconnectIntervalMs = 1000;

    //==========================================================
    // 启动
    //==========================================================
    if (!sensor.start(config))
    {
        std::cerr << "启动倾角仪失败" << std::endl;
        return -1;
    }

    std::cout << "启动成功" << std::endl;

    //==========================================================
    // 读取
    //==========================================================
    for (int i = 0; i < 1000; ++i)
    {
        if (!sensor.isRunning())
        {
            std::cout << "SDK停止运行" << std::endl;
            break;
        }

        float x = 0.0f;
        float y = 0.0f;
        sensor.getAngle(x, y);

        std::cout
            << "X = "
            << x
            << " deg, "
            << "Y = "
            << y
            << " deg"
            << std::endl;


        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    //==========================================================
    // 停止
    //==========================================================
    sensor.stop();
    std::cout << "SDK stopped." << std::endl;
    return 0;
}