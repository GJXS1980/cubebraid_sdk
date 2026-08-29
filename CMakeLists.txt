cmake_minimum_required(VERSION 3.5)

project(cubebraid_sdk CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 14)

# -----------------------------------------------------------------
# 全局统一设置所有编译目标的输出目录（兼容 Debug / Release / 所有生成器）
# -----------------------------------------------------------------
set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/SDK_Demos")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIR}")


include_directories(${PROJECT_SOURCE_DIR}/include)

link_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/Release"
)

#### logger SDK 测试demo
add_executable(logger_demo ${PROJECT_SOURCE_DIR}/src/CubeBraidSDK/LoggerSDK/logger_demo.cpp)
target_link_libraries(logger_demo PRIVATE ${PROJECT_SOURCE_DIR}/lib/LoggerSDK.lib)

#### PLC SDK 测试demo
add_executable(plc_sdk_demo ${PROJECT_SOURCE_DIR}/src/CubeBraidSDK/PLC_SDK/plc_sdk_demo.cpp)
target_link_libraries(plc_sdk_demo PRIVATE ${PROJECT_SOURCE_DIR}/lib/PLC_SDK.lib)

#### sensor SDK 测试demo
add_executable(sensorsdk_demo ${PROJECT_SOURCE_DIR}/src/CubeBraidSDK/SensorSDK/sensorsdk_demo.cpp)
target_link_libraries(sensorsdk_demo PRIVATE ${PROJECT_SOURCE_DIR}/lib/InclinometerSDK.lib)

#### JsonParameter SDK 测试demo
add_executable(jsonsdk_demo ${PROJECT_SOURCE_DIR}/src/CubeBraidSDK/JsonSDK/jsonsdk_demo.cpp)
target_link_libraries(jsonsdk_demo PRIVATE ${PROJECT_SOURCE_DIR}/lib/JsonParameterSDK.lib)

#### Kawasaki robot SDK 测试demo
add_executable(kawasaki_kinematics_sdk_demo ${PROJECT_SOURCE_DIR}/src/CubeBraidSDK/KawasakiSDK/kawasaki_kinematics_sdk_demo.cpp)
target_link_libraries(kawasaki_kinematics_sdk_demo PRIVATE ${PROJECT_SOURCE_DIR}/lib/KawasakiKinematicsSDK.lib)

#### robot SDK 测试demo
add_executable(RobotSDK_demo ${PROJECT_SOURCE_DIR}/src/CubeBraidSDK/RobotSDK/RobotSDK_demo.cpp)
target_link_libraries(RobotSDK_demo PRIVATE ${PROJECT_SOURCE_DIR}/lib/RobotSDK.lib)

#### AGV SDK 测试demo
add_executable(agv_sdk_demo ${PROJECT_SOURCE_DIR}/src/CubeBraidSDK/AGV_SDK/agv_sdk_demo.cpp)
target_link_libraries(agv_sdk_demo PRIVATE ${PROJECT_SOURCE_DIR}/lib/AGV_SDK.lib)

#### Camera SDK 测试demo
add_executable(CameraSDK_demo ${PROJECT_SOURCE_DIR}/src/CubeBraidSDK/CameraSDK/CameraSDK_demo.cpp)
target_link_libraries(CameraSDK_demo PRIVATE ${PROJECT_SOURCE_DIR}/lib/CameraSDK.lib)