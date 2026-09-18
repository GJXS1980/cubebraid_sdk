#ifndef INCLINOMETER_SDK_H
#define INCLINOMETER_SDK_H

#include <string>


//======================================================================
// SDK 导出宏
//
// Windows：
//   DLL 编译时使用 __declspec(dllexport)
//   DLL 使用时使用 __declspec(dllimport)
//
// Linux：
//   SO 使用 visibility("default") 导出
//======================================================================

#ifdef _WIN32

    #ifdef INCLINOMETER_SDK_EXPORTS
        #define INCLINOMETER_API __declspec(dllexport)
    #else
        #define INCLINOMETER_API __declspec(dllimport)
    #endif

#else

    #define INCLINOMETER_API __attribute__((visibility("default")))

#endif


namespace InclinometerSDK
{

/**
 * @brief 倾角数据
 */
struct AngleData
{
    float x;
    float y;
    float time;

    AngleData()
        : x(0.0f)
        , y(0.0f)
        , time(0.0f)
    {
    }

    AngleData(
        float xAngle,
        float yAngle,
        float timestamp)
        : x(xAngle)
        , y(yAngle)
        , time(timestamp)
    {
    }
};


/**
 * @brief 倾角仪配置
 */
struct InclinometerConfig
{
    /**
     * @brief 串口
     *
     * Windows 例如：
     * "\\\\.\\COM7"
     *
     * Linux 例如：
     * "/dev/ttyUSB0"
     */
    std::string port;

    /**
     * @brief 波特率
     */
    unsigned long baudRate;

    /**
     * @brief 是否自动重连
     */
    bool autoReconnect;

    /**
     * @brief 自动重连间隔，单位：ms
     */
    int reconnectIntervalMs;

    /**
     * @brief 是否保存CSV
     */
    bool enableCsvSave;

    /**
     * @brief CSV保存目录
     *
     * Windows 例如：
     * "data\\DXL360S"
     *
     * Linux 例如：
     * "data/DXL360S"
     */
    std::string csvDirectory;

    /**
     * @brief CSV数据保存间隔，单位：ms
     */
    int saveIntervalMs;


    /**
     * @brief 默认构造函数
     *
     * 根据不同操作系统设置默认串口和CSV目录。
     */
    InclinometerConfig()
#ifdef _WIN32
        : port("\\\\.\\COM7")
        , baudRate(9600)
        , autoReconnect(true)
        , reconnectIntervalMs(1000)
        , enableCsvSave(false)
        , csvDirectory("data\\DXL360S")
        , saveIntervalMs(100)
#else
        : port("/dev/ttyUSB0")
        , baudRate(9600)
        , autoReconnect(true)
        , reconnectIntervalMs(1000)
        , enableCsvSave(false)
        , csvDirectory("data/DXL360S")
        , saveIntervalMs(100)
#endif
    {
    }
};


/**
 * @brief DXL360 倾角仪 SDK
 *
 * C++程序可以直接使用该类。
 *
 * SDK内部串口、线程、mutex等平台相关实现通过PImpl隐藏。
 *
 * Windows 和 Linux 均支持。
 *
 * Python / C# / C 等语言通过下面的 C ABI 接口调用。
 */
class INCLINOMETER_API Inclinometer
{
public:

    /**
     * @brief 构造函数
     */
    Inclinometer();


    /**
     * @brief 析构函数
     */
    ~Inclinometer();


    /**
     * @brief 禁止拷贝
     */
    Inclinometer(const Inclinometer&) = delete;


    /**
     * @brief 禁止赋值
     */
    Inclinometer& operator=(const Inclinometer&) = delete;


    /**
     * @brief 启动倾角仪
     *
     * @param config 倾角仪配置
     *
     * @return true 启动成功
     * @return false 启动失败
     */
    bool start(const InclinometerConfig& config);


    /**
     * @brief 停止倾角仪
     */
    void stop();


    /**
     * @brief 判断SDK是否运行
     *
     * @return true 正在运行
     * @return false 未运行
     */
    bool isRunning() const;


    /**
     * @brief 获取最新X方向角度
     *
     * @return X方向角度，单位：度
     */
    float getXAngle() const;


    /**
     * @brief 获取最新Y方向角度
     *
     * @return Y方向角度，单位：度
     */
    float getYAngle() const;


    /**
     * @brief 同时获取X/Y角度
     *
     * @param x X方向角度输出
     * @param y Y方向角度输出
     */
    void getAngle(float& x, float& y) const;


    /**
     * @brief 重置当前角度
     */
    void resetAngle();


private:

    /**
     * @brief PImpl实现
     *
     * 隐藏SDK内部平台相关实现，包括：
     *
     * Windows：
     *   - Windows串口HANDLE
     *   - Windows串口API
     *
     * Linux：
     *   - Linux串口文件描述符
     *   - termios串口配置
     *
     * 公共实现：
     *   - 工作线程
     *   - mutex
     *   - 倾角数据解析
     *   - CSV数据保存
     */
    class Impl;

    Impl* m_impl;
};


} // namespace InclinometerSDK



//======================================================================
// C ABI
//
// 给 Python / C# / C 等语言调用
//
// 注意：
// C ABI接口不依赖C++类，可以通过动态库直接调用。
//======================================================================

#ifdef __cplusplus
extern "C"
{
#endif


/**
 * @brief 创建SDK对象
 *
 * @return SDK对象句柄
 */
INCLINOMETER_API void* Inclinometer_Create();


/**
 * @brief 销毁SDK对象
 *
 * @param handle SDK对象句柄
 */
INCLINOMETER_API void Inclinometer_Destroy(void* handle);


/**
 * @brief 启动倾角仪
 *
 * @param handle SDK对象句柄
 * @param port 串口
 *
 * Windows 例如：
 * "\\\\.\\COM7"
 *
 * Linux 例如：
 * "/dev/ttyUSB0"
 *
 * @param baudRate 波特率，例如 9600
 * @param autoReconnect 是否自动重连
 *
 * @return 1 成功
 * @return 0 失败
 */
INCLINOMETER_API int Inclinometer_Start(
    void* handle,
    const char* port,
    int baudRate,
    int autoReconnect);


/**
 * @brief 停止倾角仪
 *
 * @param handle SDK对象句柄
 */
INCLINOMETER_API void Inclinometer_Stop(void* handle);


/**
 * @brief 判断SDK是否运行
 *
 * @param handle SDK对象句柄
 *
 * @return 1 正在运行
 * @return 0 未运行
 */
INCLINOMETER_API int Inclinometer_IsRunning(void* handle);


/**
 * @brief 获取X方向角度
 *
 * @param handle SDK对象句柄
 *
 * @return X角度，单位：度
 */
INCLINOMETER_API float Inclinometer_GetXAngle(void* handle);


/**
 * @brief 获取Y方向角度
 *
 * @param handle SDK对象句柄
 *
 * @return Y角度，单位：度
 */
INCLINOMETER_API float Inclinometer_GetYAngle(void* handle);


/**
 * @brief 同时获取X/Y角度
 *
 * @param handle SDK对象句柄
 * @param x X角度输出
 * @param y Y角度输出
 */
INCLINOMETER_API void Inclinometer_GetAngle(
    void* handle,
    float* x,
    float* y);


/**
 * @brief 重置角度
 *
 * @param handle SDK对象句柄
 */
INCLINOMETER_API void Inclinometer_ResetAngle(void* handle);


/**
 * @brief 获取SDK版本
 *
 * @return SDK版本字符串
 */
INCLINOMETER_API const char* Inclinometer_GetVersion();


#ifdef __cplusplus
}
#endif


#endif // INCLINOMETER_SDK_H
