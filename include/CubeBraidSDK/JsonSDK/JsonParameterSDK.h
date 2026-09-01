#ifndef JSONPARAMETER_SDK_H
#define JSONPARAMETER_SDK_H
#include <fstream>

// ============================================================
// DLL 导出宏
// ============================================================
// Windows:
//   编译 SDK DLL 时定义 PARAMETER_SDK_EXPORTS，导出函数。
//   使用 SDK DLL 时不定义，导入函数。
//
// Linux:
//   当前直接使用空定义。
// ============================================================
#ifdef _WIN32
    #ifdef PARAMETER_SDK_EXPORTS
        #define JSONPARAMETER_SDK_API __declspec(dllexport)
    #else
        #define JSONPARAMETER_SDK_API __declspec(dllimport)
    #endif
#else
    #define JSONPARAMETER_SDK_API
#endif


// ============================================================
// C 接口
// ============================================================
// 使用 extern "C" 防止 C++ 名称修饰，便于：
// 1. C++ 调用
// 2. Python ctypes 调用
// 3. 其他语言动态加载 DLL
// ============================================================

#ifdef __cplusplus
extern "C" 
{
#endif

// ============================================================
// 错误码
// ============================================================
enum JsonParameterSDKError
{
    // 操作成功
    JSONPARAM_SDK_SUCCESS              = 0,

    // 参数错误
    JSONPARAM_SDK_ERROR_UNKNOWN        = -1,

    // 参数错误，例如空指针
    JSONPARAM_SDK_ERROR_INVALID_PARAM  = -2,

    // 文件不存在或无法打开
    JSONPARAM_SDK_ERROR_FILE_NOT_FOUND = -3,

    // JSON 解析失败
    JSONPARAM_SDK_ERROR_JSON_PARSE     = -4,

    // JSON 内容为空
    JSONPARAM_SDK_ERROR_JSON_EMPTY     = -5,

    // JSON 缺少字段
    JSONPARAM_SDK_ERROR_FIELD_MISSING  = -6,

    // 索引非法
    JSONPARAM_SDK_ERROR_INDEX_INVALID  = -7,

    // 用户提供的字符串缓冲区过小
    JSONPARAM_SDK_ERROR_BUFFER_SMALL   = -8,

    // 文件写入失败
    JSONPARAM_SDK_ERROR_WRITE_FAILED   = -9,

    // TXT配置文件错误
    JSONPARAM_SDK_ERROR_CONFIG = -10
};


// ============================================================
// 数据结构
// ============================================================

// 手眼标定数据
typedef struct CalibrationPose
{
    // 手眼标定位置
    float x;
    float y;
    float z;

    // 手眼标定姿态(四元数)
    float qw;
    float qx;
    float qy;
    float qz;

} CalibrationPose;

// SKU 数据
typedef struct SkuData
{
    double length;  // 产品长(mm)
    double width;   // 产品宽(mm)
    double height;  // 产品高(mm)
    double weight;  // 产品重量(kg)
} SkuData;


// 机器人位姿
typedef struct RobotPose
{
    // 机器人位置
    float x;
    float y;
    float z;

    // 机器人姿态
    float rx;
    float ry;
    float rz;
} RobotPose;


// 续码配置
typedef struct ContinuationConfig
{
    // 面数
    int surface_num;

    // 层数
    int layer_num;

    // 工作模式
    int work_mode_num;

    // AGV模式
    int agv_mode_num;

    // 动作数量
    int action_num;

    // 码垛类型(预留)
    int stack_type;
} ContinuationConfig;

// 机器人状态
typedef struct RobotState
{
    // 工作模式
    int work_mode_state;

    // 当前面状态
    int surface_state;

    // 当前层状态
    int layer_state;

    // 当前动作状态
    int action_state;

    // 总数量
    int total_num;

    // 机器人当前位置
    float x;
    float y;
    float z;

    // 机器人当前姿态
    float rx;
    float ry;
    float rz;

} RobotState;


// ============================================================
// SDK 生命周期
// ============================================================

// 初始化 SDK
JSONPARAMETER_SDK_API
int JsonParameterSDK_Initialize();

// 释放 SDK
JSONPARAMETER_SDK_API
void JsonParameterSDK_Uninitialize();

// ============================================================
// 获取最后一次错误信息
// ============================================================
//
// buffer:
//     用户提供的输出缓冲区
//
// buffer_size:
//     缓冲区大小
//
// 返回值:
//     JSONPARAM_SDK_SUCCESS
//     JSONPARAM_SDK_ERROR_INVALID_PARAM
//     JSONPARAM_SDK_ERROR_BUFFER_SMALL
// ============================================================
JSONPARAMETER_SDK_API
int JsonParameterSDK_GetLastError(char* buffer, int buffer_size);

// 检查 JSON 文件
JSONPARAMETER_SDK_API
int JsonParameterSDK_CheckJsonFile(const char* file_path);

// 获取手眼标定数据
JSONPARAMETER_SDK_API
int JsonParameterSDK_GetCalibration(const char* file_path, int cam_mode, CalibrationPose* result);

// 获取 SKU 数据
JSONPARAMETER_SDK_API
int JsonParameterSDK_GetSku(const char* file_path, int sku_index, SkuData* result);

// 获取 AGV 航向角
JSONPARAMETER_SDK_API
int JsonParameterSDK_GetAgvAngle(const char* file_path, int angle_index, float* angle);

// 获取机器人取料/放料位姿
JSONPARAMETER_SDK_API
int JsonParameterSDK_GetRobotPose(const char* file_path, int param_mode, RobotPose* result);

// 获取倾角仪端口
JSONPARAMETER_SDK_API
int JsonParameterSDK_GetInclinometerPort(const char* file_path, int port_index, std::string& port); 

// 获取续码面数/层数
JSONPARAMETER_SDK_API
int JsonParameterSDK_GetContinuationSurfaceLayer(const char* file_path, int index, int* layer_num, int* surface_num);

// 获取机器人状态
JSONPARAMETER_SDK_API
int JsonParameterSDK_GetRobotState(const char* file_path, RobotState* result);

// 初始化机器人状态文件
JSONPARAMETER_SDK_API
int JsonParameterSDK_InitRobotData(const char* file_path);

// 修改 JSON 字段
JSONPARAMETER_SDK_API
int JsonParameterSDK_SetJsonInt(const char* file_path, const char* field_name, int value);

JSONPARAMETER_SDK_API
int JsonParameterSDK_SetJsonFloat(const char* file_path, const char* field_name, float value);

JSONPARAMETER_SDK_API
int JsonParameterSDK_SetJsonString(const char* file_path, const char* field_name, const char* value);

// 获取 TXT 续码配置
JSONPARAMETER_SDK_API
int JsonParameterSDK_GetContinuationConfig(const char* file_path, ContinuationConfig* result);

// 获取垛型特殊面 diff_x
JSONPARAMETER_SDK_API
int JsonParameterSDK_GetStackStyleDiffX(const char* file_path, int index, float* diff_x);


#ifdef __cplusplus
}
#endif

#endif // JSONPARAMETER_SDK_H