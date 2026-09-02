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

    // 读取垛型json文件
    PalletizingPatternData palletizingPatternData;
    ret = JsonParameterSDK_GetPalletizingPatternData("./data/json/rd_demo_data_keba.json", 2, &palletizingPatternData);
    if (ret == JSONPARAM_SDK_SUCCESS)
    {
        std::cout << "面数: " << palletizingPatternData.number_of_surface << std::endl;
        std::cout << "层数: " << palletizingPatternData.number_of_layers << std::endl;
        std::cout << "夹具取料模式: " << palletizingPatternData.fetch_mode << std::endl;

        std::cout << "顶吸(1)/侧吸(2)模式: " << palletizingPatternData.mode_switch << std::endl;
        std::cout << "每层最左侧垛型: " << palletizingPatternData.r_offset << std::endl;
        std::cout << "正常码法(1)/特殊码法第一面最上面一层右侧两个横纸箱特殊码法(2): " << palletizingPatternData.product_type << std::endl;

        std::cout << "正常码法(0)/一横多竖特殊码法(1): " << palletizingPatternData.special_stack_type << std::endl;
        std::cout << "垛型在x方向偏移值: " << palletizingPatternData.x << std::endl;
        std::cout << "垛型在y方向偏移值: " << palletizingPatternData.y << std::endl;

        std::cout << "垛型在z方向偏移值: " << palletizingPatternData.z << std::endl;
        std::cout << "产品长: " << palletizingPatternData.sku_l << std::endl;
        std::cout << "产品宽: " << palletizingPatternData.sku_w << std::endl;

        std::cout << "产品高: " << palletizingPatternData.sku_h << std::endl;
        std::cout << "产品重量: " << palletizingPatternData.sku_weight << std::endl;
        std::cout << "集装箱长: " << palletizingPatternData.container_l << std::endl;

        std::cout << "集装箱宽: " << palletizingPatternData.container_w << std::endl;
        std::cout << "集装箱高: " << palletizingPatternData.container_h << std::endl;
        std::cout << "单次动作抓取产品数量(单位:箱): " << palletizingPatternData.sku_num << std::endl;

    }
    else
    {   
        std::cout << "JSON 文件读取失败" << std::endl;
    }

    JsonParameterSDK_Uninitialize();

    return 0;
}