#ifndef ROBOT_SDK_H
#define ROBOT_SDK_H

#include <vector>
#include <string>
#include <memory>

// 动态库导出宏定义
#if defined(_WIN32) || defined(_WIN64)
    #ifdef ROBOT_SDK_EXPORTS
        #define ROBOT_API __declspec(dllexport)
    #else
        #define ROBOT_API __declspec(dllimport)
    #endif
#else
    #define ROBOT_API __attribute__((visibility("default")))
#endif

namespace robot_sdk 
{

    /**
    * @brief 机器人位姿结构体 (XYZOAT)
    */
    struct Pose 
    {
        double x{0.0}, y{0.0}, z{0.0};
        double rx{0.0}, ry{0.0}, rz{0.0}; // O, A, T / 欧拉角
    };

    /**
    * @brief 机器人关节角度结构体 (J1-J6)
    */
    struct Joint 
    {
        double j1{0.0}, j2{0.0}, j3{0.0};
        double j4{0.0}, j5{0.0}, j6{0.0};
    };

    /**
    * @brief 产品/SKU 尺寸结构体
    */
    struct BoxDimension 
    {
        float length{0.0f}; // 长 (mm)
        float width{0.0f};  // 宽 (mm)
        float height{0.0f}; // 高 (mm)
    };

    /**
    * @brief 控制模式枚举
    */
    enum class ControlMode 
    {
        Coarse = 1,      // 粗略到位 / 过渡点 (精细度 50/200mm)
        Fine = 2,        // 精准到位 / 目标点 (精细度 1mm)
        Transition = 3   // 远距离过渡点 (精细度 200mm)
    };

    /**
    * @brief 机器人控制与抓取算法 SDK 主类
    */
    class ROBOT_API RobotController 
    {
    public:
        RobotController();
        ~RobotController();

        // ==========================================
        // 机器人连接与生命周期管理
        // ==========================================
        
        /**
        * @brief 机器人连接函数
        * @param ip 机器人 IP 地址
        * @param motionPort 控制端口 (默认 31400)
        * @param statusPort 监听端口 (默认 31401)
        * @return bool 连接是否成功
        */
        bool connectRobot(const std::string& ip, int motionPort = 31400, int statusPort = 31401);

        /**
        * @brief 断开机器人连接并释放套接字
        */
        void disconnectRobot();

        /**
        * @brief 判断机械臂连接是否正常
        * @return bool 连接状态
        */
        bool isConnected() const;

        // ==========================================
        // 机器人运动控制接口
        // ==========================================

        /**
        * @brief 机器人位姿控制函数
        * @param mode 控制模式 (1: 过渡点50mm, 2: 目标点1mm, 3: 过渡点200mm)
        * @param pose 机器人在基坐标系下的目标位姿 (x, y, z, O, A, T)
        * @return bool 发送及到位回应是否成功
        */
        bool controlPosture(ControlMode mode, const Pose& pose);

        /**
        * @brief 机器人关节控制函数
        * @param mode 控制模式 (1: 过渡点, 2: 目标点)
        * @param joint 机器人 6 个关节的目标角度
        * @return bool 发送及到位回应是否成功
        */
        bool controlJoint(ControlMode mode, const Joint& joint);

        // ==========================================
        // 抓取/码垛算法与斜面补偿
        // ==========================================

        /**
        * @brief 顶吸数据斜面 x 方向补偿
        * @param benchmark_x 基准点 x 坐标值（单位: mm）
        * @param angleDeg 斜面倾斜值（单位: °）
        * @param h 产品码垛高度值（单位: mm）
        * @param x0 码垛点距离基准点坐标 x 的相对偏移量（单位: mm）
        * @return float 机器人实际码垛 x 值
        */
        static float top_x_value(float benchmark_x, float angleDeg, float h, float x0);

        /**
        * @brief 顶吸数据斜面 z 方向补偿
        * @param angleDeg 斜面倾斜值（单位: °）
        * @param h 产品码垛高度值（单位: mm）
        * @param x0 码垛点距离基准点坐标 x 的相对偏移量（单位: mm）
        * @return float 机器人实际码垛 z 补偿值
        */
        static float top_z_value(float angleDeg, float h, float x0);

        /**
        * @brief 侧吸数据斜面 x 方向补偿
        * @param benchmark_x 基准点 x 坐标值（单位: mm）
        * @param angleDeg 斜面倾斜值（单位: °）
        * @param h 产品码垛高度值（单位: mm）
        * @param x0 码垛点距离基准点坐标 x 的相对偏移量（单位: mm）
        * @return float 机器人实际码垛 x 值
        */
        static float side_x_value(float benchmark_x, float angleDeg, float h, float x0);

        /**
        * @brief 侧吸数据斜面 z 方向补偿
        * @param angleDeg 斜面倾斜值（单位: °）
        * @param h 产品码垛高度值（单位: mm）
        * @param x0 码垛点距离基准点坐标 x 的相对偏移量（单位: mm）
        * @return float 机器人实际码垛 z 补偿值
        */
        static float side_z_value(float angleDeg, float h, float x0);

        /**
        * @brief 顶吸偏移量 xyz 求解（带倾角补偿）
        * @param centroid 基准点坐标 (米，算法内自动转 mm)
        * @param box SKU 尺寸 (长, 宽, 高，单位: mm)
        * @param fetchMode 抓取模式 (1,2: 吸取长边; 3,4: 吸取短边)
        * @param sku_num 产品数量
        * @param dis_y 每层剩余缝隙
        * @param poseOffset 相机/JSON 偏移数据 (x, y, z，单位: mm)
        * @param ROffset 左侧/右侧标志位 (true: 左侧, false: 右侧)
        * @param model_mod 模型模式
        * @param inclx_angle 斜面倾斜角度 (单位: °)
        * @return Pose 计算后的机器人目标位姿 (x, y, z)
        */
        static Pose Top_suction_angle(
            const Pose& centroid, 
            const BoxDimension& box, 
            int fetchMode, 
            int sku_num, 
            float dis_y, 
            const Pose& poseOffset, 
            bool ROffset, 
            int model_mod, 
            double inclx_angle = 0.0
        );

    private:
        class Impl;
        std::unique_ptr<Impl> pImpl; // 使用 PImpl 隐藏 Socket 及网络平台实现
    };

} // namespace robot_sdk

// ==========================================
// C API 导出接口 (支持 C# / Python / C 调用)
// ==========================================
#ifdef __cplusplus
extern "C" 
{
    #endif

    typedef void* RobotHandle;

    ROBOT_API RobotHandle Robot_Create();
    ROBOT_API void Robot_Destroy(RobotHandle handle);
    ROBOT_API int Robot_Connect(RobotHandle handle, const char* ip, int motionPort, int statusPort);
    ROBOT_API void Robot_Disconnect(RobotHandle handle);
    ROBOT_API int Robot_ControlPosture(RobotHandle handle, int mode, double x, double y, double z, double rx, double ry, double rz);
    ROBOT_API int Robot_ControlJoint(RobotHandle handle, int mode, double j1, double j2, double j3, double j4, double j5, double j6);

    #ifdef __cplusplus
}
#endif

#endif // ROBOT_SDK_H