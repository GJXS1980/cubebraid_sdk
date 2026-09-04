#ifndef AGV_CONTROLLER_H
#define AGV_CONTROLLER_H

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <memory>
#include <stdint.h>
#include <stdbool.h>

// ============================================================================
// Windows / Linux 动态库导出宏定义
// ============================================================================
#ifdef _WIN32
    #ifdef AGV_SDK_EXPORTS
        #define AGV_API __declspec(dllexport)
    #else
        #define AGV_API __declspec(dllimport)
    #endif
#else
    #define AGV_API __attribute__((visibility("default")))
#endif

// 包含 Protobuf 和协议 Frame 头文件 (仅 C++ 编译可见，避免纯 C 编译报错)
#ifdef __cplusplus
#include "CubeBraidSDK/AGV_SDK/main.pb.h"
#include "CubeBraidSDK/AGV_SDK/Frame.h"
#endif

namespace agv_sdk 
{
    /**
     * @brief AGV 位置与姿态结构体 (6自由度)
     */
    struct AGVPose 
    {
        double x{0.0};
        double y{0.0};
        double z{0.0};
        double roll{0.0};  // 前方距离 (m)
        double pitch{0.0}; // 左侧距离 (m)
        double yaw{0.0};
    };

    /**
     * @brief AGV 控制模式(手动/自动模式)
     */
    enum class ControlMode 
    {
        Manual = 2, // 手动模式
        Auto = 3    // 自动模式
    };

    // 为类的导出添加 AGV_API 声明
    class AGV_API AGVController 
    {
    public:
        AGVController();
        ~AGVController();

        // C++14 显式禁用拷贝构造与赋值
        AGVController(const AGVController&) = delete;
        AGVController& operator=(const AGVController&) = delete;

        /**
         * @brief 连接 AGV 服务器
         * @param ip IP 地址
         * @param port 端口号
         * @return 是否连接成功
         */
        bool connectAGV(const std::string& ip, int port);

        /**
         * @brief 断开网络连接并清理资源
         */
        void disconnectAGV();

        /**
         * @brief 登录 AGV 系统
         * @param username 用户名
         * @param password_hash 哈希加密后的密码
         * @return 登录结果
         */
        bool login(const std::string& username, const std::string& password_hash);

        /**
         * @brief 注销登录
         * @return 注销结果
         */
        bool logout();

        // 控制指令
        /**
         * @brief 切换 AGV 控制模式 (手动 / 自动)
         */
        bool switchControlMode(ControlMode mode);

        /**
         * @brief 自动模式：控制 AGV 前进到底指定距离 (阻塞至完成或超时)
         * @param dist_mm 前进距离 (单位: 毫米)
         * @param timeout_ms 超时时间 (单位: 毫秒，默认 30 秒)
         */
        bool goForward(double dist_mm, int timeout_ms = 30000);

        /**
         * @brief 自动模式：控制 AGV 单步前进/单步后退指定距离 (阻塞至完成或超时)
         * @param dist_mm 后退距离 (单位: 毫米)
         * @param timeout_ms 超时时间 (单位: 毫秒，默认 30 秒)
         */
        bool goBack(double dist_mm, int timeout_ms = 30000);

        /**
         * @brief 单次发送手动速度控制指令（需要在手动模式下）
         * @param vx X轴线速度 (单位: mm/s)
         * @param vy Y轴线速度 (单位: mm/s)
         * @param w 角速度 (单位: 0.001 rad/s)
         */
        bool manualCtlVelSet(float vx, float vy, float w);

        // 状态查询
        /**
         * @brief 主动向 AGV 发送系统状态查询请求
         */
        bool querySystemState();

        /**
         * @brief 主动发送心跳消息
         */
        bool sendHeartBeatsMsg(); // 心跳包

        /**
         * @brief 获取本地缓存的 AGV 最新位姿 (线程安全)
         */
        AGVPose getPose() const;

        /**
         * @brief 手动控制：在指定时长内持续以固定频率重发速度，到期后自动发送停止指令 (速度归零)
         * @param vx X轴线速度 (mm/s)
         * @param vy Y轴线速度 (mm/s)
         * @param w 角速度 (0.001 rad/s)
         * @param duration_ms 持续总时间 (毫秒)
         * @param interval_ms 速度指令重发时间间隔 (毫秒，默认 100ms)
         */
        bool moveManualForDuration(float vx, float vy, float w, int duration_ms, int interval_ms = 100);

    private:
        // --------------------------------------------------------------------
        // 内部后台工作线程函数
        // --------------------------------------------------------------------
        void receiveLoop();     // TCP 报文接收与拆包循环线程
        void heartbeatLoop();      // 自动心跳包维持循环线程
        
        // --------------------------------------------------------------------
        // 数据流拆包与消息解包处理
        // --------------------------------------------------------------------
        void parseDataStream(const char* data, size_t size);        // 处理粘包/半包解析
        void processMessage(proto::Message_ptr msg);                // 分发处理业务层消息
        
        // --------------------------------------------------------------------
        // 网络底层数据包打包与发送
        // --------------------------------------------------------------------
        bool sendDataFrame(proto::Frame_ptr msg);       // 底层 Socket 基础发送函数
        void sendNotifyAck();                           // 发送通知应答

    private:
        // --------------------------------------------------------------------
        // 网络句柄与运行状态标志
        // --------------------------------------------------------------------
        int m_sockfd{-1};   // 套接字描述符
        std::atomic<bool> m_connected{false};   // 网络连接状态标志位 (线程安全)
        std::atomic<bool> m_running{false};     // 后台工作线程运行状态标志位 (线程安全）
        
        // --------------------------------------------------------------------
        // 协议序号与会话 ID (使用原子变量保证自增与读写的线程安全)
        // --------------------------------------------------------------------
        std::atomic<uint64_t> m_sessionId{0};   // 系统级唯一会话句柄
        std::atomic<int> m_nSeqNo{0};           // 协议包递增序号
        std::atomic<int> m_nSessionId{1};       // 业务包会话编号

        // --------------------------------------------------------------------
        // 位姿数据及其互斥锁
        // --------------------------------------------------------------------
        mutable std::mutex m_poseMutex;         // 保护位姿数据的互斥锁
        AGVPose m_currentPose;                  // 缓存的最新位姿

        // --------------------------------------------------------------------
        // 异步控制命令的条件变量与同步信号
        // --------------------------------------------------------------------
        std::mutex m_taskMutex;                 // 保护控制任务完成标志的互斥锁
        std::condition_variable m_taskCv;       // 用于阻塞等待指令完成的条件变量
        bool m_forwardFinished{false};          // 前进指令是否执行完毕
        bool m_backFinished{false};             // 后退指令是否执行完毕

        // --------------------------------------------------------------------
        // 网络接收流缓冲区
        // --------------------------------------------------------------------
        std::vector<char> m_recvBuf;            // TCP 数据接收流环形/动态缓冲区 (仅在 receiveLoop 线程中访问)

        // --------------------------------------------------------------------
        // 后台工作线程对象
        // --------------------------------------------------------------------
        std::thread m_recvThread;       // 数据接收线程
        std::thread m_heartbeatThread;  // 心跳维持线程
    };

} // namespace agv_sdk

// ============================================================================
// C-API 导出接口 (用于 Python ctypes / C 语言等跨语言调用)
// ============================================================================

typedef void* AGV_Handle;

/**
 * @brief C 兼容接口使用的位姿结构体
 */
typedef struct 
{
    double x;
    double y;
    double z;
    double roll;
    double pitch;
    double yaw;
} C_AGVPose;

/**
 * @brief C 兼容接口使用的控制模式枚举
 */
typedef enum 
{
    C_CONTROL_MODE_MANUAL = 2,
    C_CONTROL_MODE_AUTO   = 3
} C_ControlMode;

#ifdef __cplusplus
extern "C" 
{
#endif

// 句柄创建与销毁
AGV_API AGV_Handle AGV_Create();
AGV_API void       AGV_Destroy(AGV_Handle handle);

// 网络与会话管理
AGV_API bool       AGV_Connect(AGV_Handle handle, const char* ip, int port);
AGV_API void       AGV_Disconnect(AGV_Handle handle);
AGV_API bool       AGV_Login(AGV_Handle handle, const char* username, const char* password_hash);
AGV_API bool       AGV_Logout(AGV_Handle handle);

// 控制指令接口
AGV_API bool       AGV_SwitchControlMode(AGV_Handle handle, C_ControlMode mode);
AGV_API bool       AGV_GoForward(AGV_Handle handle, double dist_mm, int timeout_ms);
AGV_API bool       AGV_GoBack(AGV_Handle handle, double dist_mm, int timeout_ms);
AGV_API bool       AGV_ManualCtlVelSet(AGV_Handle handle, float vx, float vy, float w);

// 状态与查询接口
AGV_API bool       AGV_QuerySystemState(AGV_Handle handle);
AGV_API bool       AGV_SendHeartBeatsMsg(AGV_Handle handle);
AGV_API C_AGVPose  AGV_GetPose(AGV_Handle handle);

#ifdef __cplusplus
}
#endif

#endif // AGV_CONTROLLER_H