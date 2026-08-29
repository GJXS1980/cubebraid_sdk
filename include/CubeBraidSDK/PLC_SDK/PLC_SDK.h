#ifndef PLC_SDK_H
#define PLC_SDK_H

#include <stdint.h>
#include <stdbool.h>

// 导出/导入宏定义 (支持 Windows DLL / Linux SO)
#if defined(_WIN32) || defined(_WIN64)
    #if defined(PLC_SDK_EXPORTS)
        #define PLC_SDK_API __declspec(dllexport)
    #else
        #define PLC_SDK_API __declspec(dllimport)
    #endif
#else
    #define PLC_SDK_API __attribute__((visibility("default")))
#endif

// =================================================================
// 1. C / C++ 通用数据结构定义
// =================================================================
#pragma pack(push, 1)

// PLC 读取状态结构体
typedef struct 
{
    int16_t suction_cup_state;            // 吸盘状态
    int16_t fixture_state;                // 夹具/底托状态
    int16_t robot_pick_state;             // 机器人取料状态
    int16_t robot_leave_state;            // 机器人离开状态
    int16_t table_control_state;          // 摆台控制状态
    int16_t hydraulic_rod_rise_state;     // 液压杆上升状态
    int16_t hydraulic_rod_lower_state;    // 液压杆下降状态
    int16_t plc_init_state;               // PLC初始化状态
    int16_t table_init_control_ok_state;  // 摆台初始化完成状态
    int16_t plc_data_request_state;       // PLC数据请求状态
} PLCStatus;

// 取料参数输入结构体
typedef struct 
{
    int fetch_mode;       // 取料模式 (1, 2 为转动；3, 4 为不转动)
    int mode_switch;      // 模式切换参数
    double sku_l;         // 长 (mm)
    double sku_w;         // 宽 (mm)
    double sku_h;         // 高 (mm)
    int sku_num;          // 数量
    double sku_weight;    // 单个重量 (kg)
} PickUpData;

#pragma pack(pop)

// C API 句柄定义
typedef void* PLC_HANDLE;
// =================================================================
// 2. 导出 C-API 接口 (支持 C/C++ 及其他语言调用)
// =================================================================
#ifdef __cplusplus
extern "C" 
{
    #endif

    /**
    * @brief 创建 PLC 控制器实例句柄
    * @return PLC_HANDLE 成功返回非空句柄，失败返回 NULL
    */
    PLC_SDK_API PLC_HANDLE plc_create(void);

    /**
    * @brief 销毁 PLC 控制器实例句柄
    * @param handle 控制器句柄
    */
    PLC_SDK_API void plc_destroy(PLC_HANDLE handle);

    /**
    * @brief 连接 PLC
    * @param handle 控制器句柄
    * @param ip PLC IP 地址
    * @param rack 机架号
    * @param slot 插槽号
    * @return bool 是否连接成功
    */
    PLC_SDK_API bool plc_connect(PLC_HANDLE handle, const char* ip, int rack, int slot);

    /**
    * @brief 断开 PLC 连接
    * @param handle 控制器句柄
    */
    PLC_SDK_API void plc_disconnect(PLC_HANDLE handle);

    /**
    * @brief 查询 PLC 连接状态
    * @param handle 控制器句柄
    * @return bool 是否处于连接状态
    */
    PLC_SDK_API bool plc_is_connected(PLC_HANDLE handle);

    /**
    * @brief 安全写入 PLC DB 块
    * @param handle 控制器句柄
    * @param dbNumber DB 块编号
    * @param start 起始地址
    * @param size 写入字节数
    * @param buffer 数据缓冲区指针
    * @return bool 是否写入成功
    */
    PLC_SDK_API bool plc_safe_db_write(PLC_HANDLE handle, int dbNumber, int start, int size, void* buffer);

    /**
    * @brief 安全读取 PLC DB 块
    * @param handle 控制器句柄
    * @param dbNumber DB 块编号
    * @param start 起始地址
    * @param size 读取字节数
    * @param buffer 接收缓冲区指针
    * @return bool 是否读取成功
    */
    PLC_SDK_API bool plc_safe_db_read(PLC_HANDLE handle, int dbNumber, int start, int size, void* buffer);

    // 控制离散信号设置接口
    PLC_SDK_API void plc_set_open_suction_cup(PLC_HANDLE handle, bool enable);
    PLC_SDK_API void plc_set_suction_cup_picking_ok(PLC_HANDLE handle, bool enable);
    PLC_SDK_API void plc_set_fixture_roll_out(PLC_HANDLE handle, bool enable);
    PLC_SDK_API void plc_set_retrieving_completion_side(PLC_HANDLE handle, bool enable);
    PLC_SDK_API void plc_set_fixture_initialization(PLC_HANDLE handle, bool enable);
    PLC_SDK_API void plc_set_close_suction_cup(PLC_HANDLE handle, bool enable);
    PLC_SDK_API void plc_set_inclinometer_error_signal(PLC_HANDLE handle, bool enable);

    // 业务数据与状态控制接口
    PLC_SDK_API bool plc_get_status(PLC_HANDLE handle, PLCStatus* status_out);
    PLC_SDK_API void plc_control_table_angle(PLC_HANDLE handle, int angle_mode);
    PLC_SDK_API void plc_update_box_state(PLC_HANDLE handle, int surface_num, int lay_num, int box_num);
    PLC_SDK_API void plc_update_inclinometer_angle(PLC_HANDLE handle, float angle_value);
    PLC_SDK_API void plc_update_total_box_state(PLC_HANDLE handle, int totalbox_num, int remain_num);
    PLC_SDK_API bool plc_send_pickup_data(PLC_HANDLE handle, const PickUpData* data);

    // 触发初始化接口
    PLC_SDK_API bool plc_trigger_plc_init(PLC_HANDLE handle);
    PLC_SDK_API bool plc_trigger_table_init(PLC_HANDLE handle);

    #ifdef __cplusplus
}
#endif

// =================================================================
// 3. 原生 C++ 类接口 (面向 C++ 开发者直接使用)
// =================================================================
#ifdef __cplusplus

#include "CubeBraidSDK/PLC_SDK/snap7.h"
#include <string>
#include <thread>
#include <mutex>
#include <atomic>

namespace plc_sdk 
{
    class PLC_SDK_API PLCController 
    {
    public:
        PLCController();
        ~PLCController();

        bool connect(const std::string& ip, int rack = 0, int slot = 1);
        void disconnect();
        bool isConnected() const;

        // 基础 DB 块读写接口
        bool safeDBWrite(int dbNumber, int start, int size, void* buffer);
        bool safeDBRead(int dbNumber, int start, int size, void* buffer);

        // 控制离散信号设置接口
        void setOpenSuctionCup(bool enable);
        void setSuctionCupPickingOK(bool enable);
        void setFixtureRollOut(bool enable);
        void setRetrievingCompletionSide(bool enable);
        void setFixtureInitialization(bool enable);
        void setCloseSuctionCup(bool enable);
        void setInclinometerErrorSignal(bool enable);

        // 业务与状态数据控制接口
        PLCStatus getStatus();
        void controlTableAngle(int angle_mode);
        void updateBoxState(int surface_num, int lay_num, int box_num);
        void updateInclinometerAngle(float angle_value);
        void updateTotalBoxState(int totalbox_num, int remain_num);
        
        /**
         * @brief 发送取料数据到 PLC (DB22, 偏移 0)
         * @param data 取料参数结构体
         * @return bool 是否成功写入
         */
        bool sendPickUpData(const PickUpData& data);

        // 初始化触发接口
        bool triggerPLCInit();
        bool triggerTableInit();

    private:
        int16_t swapBigEndianToHost(const uint8_t* buffer);
        void readLoop();
        void writeLoop();
        void monitorLoop();

        TS7Client client_;
        std::string ip_;
        int rack_;
        int slot_;

        std::atomic<bool> is_connected_{false};
        std::atomic<bool> is_running_{false};

        // 离散控制信号标志位
        bool open_suction_cup_signal_{false};
        bool suction_cup_picking_ok_signal_{false};
        bool fixture_roll_out_signal_{false};
        bool retrieving_completion_signal_side_{false};
        bool fixture_initialization_signal_{false};
        bool close_suction_cup_signal_{false};
        bool inclinometer_error_signal_{false};

        PLCStatus current_status_{};

        mutable std::mutex client_mutex_;
        std::mutex signal_mutex_;
        std::mutex status_mutex_;

        std::thread read_thread_;
        std::thread write_thread_;
        std::thread monitor_thread_;
    };
}
#endif // __cplusplus

#endif // PLC_SDK_H