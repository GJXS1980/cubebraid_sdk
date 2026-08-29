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

    JsonParameterSDK_Uninitialize();

    return 0;
}