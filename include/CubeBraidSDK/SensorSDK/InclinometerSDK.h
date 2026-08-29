#ifndef INCLINOMETER_SDK_H
#define INCLINOMETER_SDK_H

#include <string>

#ifdef _WIN32

    #ifdef INCLINOMETER_SDK_EXPORTS
        #define INCLINOMETER_API __declspec(dllexport)
    #else
        #define INCLINOMETER_API __declspec(dllimport)
    #endif

#else

    #define INCLINOMETER_API

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
     * 例如：
     * "\\\\.\\COM7"
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
     */
    std::string csvDirectory;

    /**
     * @brief CSV数据保存间隔，单位：ms
     */
    int saveIntervalMs;

    InclinometerConfig()
        : port("\\\\.\\COM7")
        , baudRate(9600)
        , autoReconnect(true)
        , reconnectIntervalMs(1000)
        , enableCsvSave(false)
        , csvDirectory("data\\DXL360S")
        , saveIntervalMs(100)
    {
    }
};


/**
 * @brief DXL360 倾角仪 SDK
 *
 * C++程序可以直接使用该类。
 *
 * Python / C# 等语言通过下面的 C ABI 接口调用。
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
     * @param config 配置
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
     * @param x X方向角度
     * @param y Y方向角度
     */
    void getAngle(float& x, float& y) const;


    /**
     * @brief 重置当前角度
     */
    void resetAngle();


private:

    /**
     * @brief PImpl
     *
     * 隐藏Windows串口、线程、mutex等内部实现。
     */
    class Impl;

    Impl* m_impl;
};


} // namespace InclinometerSDK



//======================================================================
// C ABI
//
// 给 Python / C# / C 等语言调用
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
 * @param port 串口，例如 "\\\\.\\COM7"
 * @param baudRate 波特率，例如 9600
 * @param autoReconnect 是否自动重连
 *
 * @return 1 成功
 * @return 0 失败
 */
INCLINOMETER_API int Inclinometer_Start(void* handle, const char* port, int baudRate, int autoReconnect);

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
INCLINOMETER_API void Inclinometer_GetAngle(void* handle, float* x, float* y);


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