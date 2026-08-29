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
    struct AGVPose 
    {
        double x{0.0};
        double y{0.0};
        double z{0.0};
        double roll{0.0};  // 前方距离 (m)
        double pitch{0.0}; // 左侧距离 (m)
        double yaw{0.0};
    };

    enum class ControlMode 
    {
        Manual = 2,
        Auto = 3
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

        // 连接与会话管理
        bool connectAGV(const std::string& ip, int port);
        void disconnectAGV();
        bool login(const std::string& username, const std::string& password_hash);
        bool logout();

        // 控制指令
        bool switchControlMode(ControlMode mode);
        bool goForward(double dist_mm, int timeout_ms = 30000);
        bool goBack(double dist_mm, int timeout_ms = 30000);
        bool manualCtlVelSet(float vx, float vy, float w);

        // 状态查询
        bool querySystemState();
        bool sendHeartBeatsMsg(); // 心跳包
        AGVPose getPose() const;

    private:
        // 内部线程 loop
        void receiveLoop();
        void heartbeatLoop();
        
        // 数据流拆包与消息处理
        void parseDataStream(const char* data, size_t size);
        void processMessage(proto::Message_ptr msg);
        
        // 网络底层发送
        bool sendDataFrame(proto::Frame_ptr msg);
        void sendNotifyAck();

    private:
        int m_sockfd{-1};   // socket套接字
        std::atomic<bool> m_connected{false};
        std::atomic<bool> m_running{false};
        
        // 使用原子变量保证自增与读写的线程安全
        std::atomic<uint64_t> m_sessionId{0};
        std::atomic<int> m_nSeqNo{0};   
        std::atomic<int> m_nSessionId{1};   

        // 姿态数据与线程安全锁
        mutable std::mutex m_poseMutex;
        AGVPose m_currentPose;

        // 异步任务同步控制
        std::mutex m_taskMutex;
        std::condition_variable m_taskCv;
        bool m_forwardFinished{false};
        bool m_backFinished{false};

        // TCP 数据接收流缓冲区 (仅在 receiveLoop 线程中访问)
        std::vector<char> m_recvBuf;

        // 工作线程
        std::thread m_recvThread;
        std::thread m_heartbeatThread;
    };

} // namespace agv_sdk

// ============================================================================
// C-API 导出接口 (用于 Python ctypes 调用)
// ============================================================================

typedef void* AGV_Handle;

typedef struct 
{
    double x;
    double y;
    double z;
    double roll;
    double pitch;
    double yaw;
} C_AGVPose;

typedef enum 
{
    C_CONTROL_MODE_MANUAL = 2,
    C_CONTROL_MODE_AUTO   = 3
} C_ControlMode;

#ifdef __cplusplus
extern "C" 
{
#endif

AGV_API AGV_Handle AGV_Create();
AGV_API void       AGV_Destroy(AGV_Handle handle);

AGV_API bool       AGV_Connect(AGV_Handle handle, const char* ip, int port);
AGV_API void       AGV_Disconnect(AGV_Handle handle);
AGV_API bool       AGV_Login(AGV_Handle handle, const char* username, const char* password_hash);
AGV_API bool       AGV_Logout(AGV_Handle handle);

AGV_API bool       AGV_SwitchControlMode(AGV_Handle handle, C_ControlMode mode);
AGV_API bool       AGV_GoForward(AGV_Handle handle, double dist_mm, int timeout_ms);
AGV_API bool       AGV_GoBack(AGV_Handle handle, double dist_mm, int timeout_ms);
AGV_API bool       AGV_ManualCtlVelSet(AGV_Handle handle, float vx, float vy, float w);

AGV_API bool       AGV_QuerySystemState(AGV_Handle handle);
AGV_API bool       AGV_SendHeartBeatsMsg(AGV_Handle handle);
AGV_API C_AGVPose  AGV_GetPose(AGV_Handle handle);

#ifdef __cplusplus
}
#endif

#endif // AGV_CONTROLLER_H