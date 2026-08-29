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

    std::string inclinometerPort;
    ret = JsonParameterSDK_GetInclinometerPort("./data/json/setting_parameters.json", 5, inclinometerPort);
    if (ret == JSONPARAM_SDK_SUCCESS)
    {
        std::cout << "Inclinometer Port: " << inclinometerPort << std::endl;
    }

    int sur_num = 0;
    int layer_num = 0;
    ret = JsonParameterSDK_GetContinuationSurfaceLayer("./data/json/setting_parameters.json", 4,  &sur_num,  &layer_num);
    if (ret == JSONPARAM_SDK_SUCCESS)
    {
        std::cout << "sur_num: " << sur_num << std::endl;
        std::cout << "layer_num: " << layer_num << std::endl;
    }

    JsonParameterSDK_Uninitialize();

    return 0;
}