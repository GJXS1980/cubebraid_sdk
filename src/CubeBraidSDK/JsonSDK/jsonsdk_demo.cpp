#include "CubeBraidSDK/JsonSDK/JsonParameterSDK.h"

#include <iostream>


int main()
{
    int ret = JsonParameterSDK_Initialize();

    if (ret != JSONPARAM_SDK_SUCCESS)
    {
        return -1;
    }

    CalibrationPose pose;
    ret = JsonParameterSDK_GetCalibration("./data/json/Hand-eye_calibration_parameters.json", 0, &pose);
    if (ret == JSONPARAM_SDK_SUCCESS)
    {
        std::cout << "x = " << pose.x << std::endl;
        std::cout << "y = " << pose.y << std::endl;
        std::cout << "z = " << pose.z << std::endl;
    }

    SkuData sku;
    ret = JsonParameterSDK_GetSku("./data/json/sku_data.json", 0, &sku);
    if (ret == JSONPARAM_SDK_SUCCESS)
    {
        std::cout << "length = " << sku.length << std::endl;
        std::cout << "width = " << sku.width << std::endl;
        std::cout << "height = " << sku.height << std::endl;
    }

    RobotPose robotpose;
    ret = JsonParameterSDK_GetRobotPose("./data/json/setting_parameters.json", 1, &robotpose);
    if (ret == JSONPARAM_SDK_SUCCESS)
    {
        std::cout << "x = " << robotpose.x << std::endl;
        std::cout << "y = " << robotpose.y << std::endl;
        std::cout << "z = " << robotpose.z << std::endl;
    }

    char inclinometerPort[256] = {0};
    ret = JsonParameterSDK_GetInclinometerPort("./data/json/setting_parameters.json", 5, inclinometerPort);
    if (ret == JSONPARAM_SDK_SUCCESS)
    {
        std::cout << "Inclinometer Port: " << inclinometerPort << std::endl;
    }

    int sur_num = 0;
    int layer_num = 0;
    ret = JsonParameterSDK_GetContinuationSurfaceLayer("./data/json/rd_demo_data_keba.json", 4,  &sur_num,  &layer_num);
    if (ret == JSONPARAM_SDK_SUCCESS)
    {
        std::cout << "sur_num: " << sur_num << std::endl;
        std::cout << "layer_num: " << layer_num << std::endl;
    }

    float dis_x = 0;
    for(int i = 0; i < 50; i++)
    {
        ret = JsonParameterSDK_GetStackStyleDiffX("./data/json/test_demo.json", i,  &dis_x);
        if (ret == JSONPARAM_SDK_SUCCESS)
        {
            std::cout << "dis_x: " << dis_x << std::endl;
        }
        else
        {   
            std::cout << "error: " <<  ret << std::endl;
        }
    }

    // 读取机器人当前状态
    RobotState robotState;
    ret = JsonParameterSDK_GetRobotState("./data/json/robot_data.json", &robotState);
    if (ret == JSONPARAM_SDK_SUCCESS)
    {
        std::cout << "基准点x = " << robotState.x << std::endl;
        std::cout << "基准点 y = " << robotState.y << std::endl;
        std::cout << "基准点 z = " << robotState.z << std::endl;

        std::cout << "基准点 rx = " << robotState.rx << std::endl;
        std::cout << "基准点 ry = " << robotState.ry << std::endl;
        std::cout << "基准点 rz = " << robotState.rz << std::endl;

        std::cout << "工作模式 " << robotState.work_mode_state << std::endl;
        std::cout << "当前面状态 " << robotState.surface_state << std::endl;
        std::cout << "当前层状态 " << robotState.layer_state << std::endl;

        std::cout << "当前动作状态 " << robotState.action_state << std::endl;
        std::cout << "总数量 " << robotState.total_num << std::endl;
    }

    // 初始化机器人json文件
    ret = JsonParameterSDK_InitRobotData("./data/json/robot_data.json");
    if (ret == JSONPARAM_SDK_SUCCESS)
    {
        std::cout << "JSON 文件更新成功" << std::endl;
    }
    else
    {   
        std::cout << "JSON 文件更新失败" << std::endl;
    }

    // 读取续码txt文件
    ContinuationConfig conConfig;
    ret = JsonParameterSDK_GetContinuationConfig("./data/txt/continuation_config.txt", &conConfig);
    if (ret == JSONPARAM_SDK_SUCCESS)
    {
        std::cout << "面数: " << conConfig.surface_num << std::endl;
        std::cout << "层数: " << conConfig.layer_num << std::endl;
        std::cout << "工作模式: " << conConfig.work_mode_num << std::endl;

        std::cout << "AGV模式: " << conConfig.agv_mode_num << std::endl;
        std::cout << "动作数量: " << conConfig.action_num << std::endl;
        std::cout << "码垛类型(预留): " << conConfig.stack_type << std::endl;

    }
    else
    {   
        std::cout << "JSON 文件读取失败" << std::endl;
    }

    JsonParameterSDK_Uninitialize();

    return 0;
}