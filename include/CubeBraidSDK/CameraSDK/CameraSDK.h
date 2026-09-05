#ifndef CAMERA3D_SDK_H
#define CAMERA3D_SDK_H

#include <string>

#ifdef _WIN32
    #ifdef CAMERA3D_SDK_EXPORTS
        #define CAMERA3D_API __declspec(dllexport)
    #else
        #define CAMERA3D_API __declspec(dllimport)
    #endif
#else
    #define CAMERA3D_API __attribute__((visibility("default")))
#endif

namespace camera3d_sdk
{

    /**
     * @brief SDK执行状态码
     */
    enum class StatusCode
    {
        SUCCESS = 0,               // 执行成功
        CAMERA_CONNECT_FAILED = 1, // 相机连接失败
        CAMERA_CAPTURE_FAILED = 2, // 图像采集失败
        INVALID_PARAMETER = 3,     // 参数错误
        CALIBRATION_FAILED = 4,    // 标定失败
        POINT_CLOUD_FAILED = 5,    // 点云处理失败
        ALGORITHM_FAILED = 6,      // 算法处理失败
        INTERNAL_ERROR = 99        // 内部错误
    };

    /**
     * @brief 三维坐标
     */
    struct Point3D
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
    };

    /**
     * @brief 手眼标定参数
     */
    struct CalibrationPose
    {
        // 手眼标定位置 (单位: 米)
        float x{0.0f};
        float y{0.0f};
        float z{0.0f};

        // 手眼标定姿态 (四元数)
        float qw{1.0f};
        float qx{0.0f};
        float qy{0.0f};
        float qz{0.0f};
    };

    /**
     * @brief 相机配置
     */
    struct CameraConfig
    {
        std::string camera_ip = "192.168.23.203";       // 下/主相机 IP
        std::string camera_ip_up = "192.168.23.88";     // 上相机 IP (装卸一体模式)

        std::string depth_file = "./data/img/depth.tiff";
        std::string color_file = "./data/img/demo.png";
    };

    /**
     * @brief Camera3D SDK主类
     */
    class CAMERA3D_API Camera3D
    {
    public:

        Camera3D();
        ~Camera3D();

        // 禁止拷贝构造和赋值（硬件 SDK 实例不可随便复制）
        Camera3D(const Camera3D&) = delete;
        Camera3D& operator=(const Camera3D&) = delete;

        // 支持移动构造与赋值
        Camera3D(Camera3D&&) noexcept;
        Camera3D& operator=(Camera3D&&) noexcept;

        StatusCode initialize(const CameraConfig& config);

        bool connect();

        void disconnect() noexcept;

        bool isConnected() const noexcept;

        std::string getLastError() const;

        StatusCode getLastStatus() const noexcept;


        /**
        * @brief 机器人在集装箱里面和斜坡上基准点的计算
        * 
        * @param calib_pose 相机手眼标定外参位姿 (包含 x, y, z, qw, qx, qy, qz)
        * @param cameraIP 相机IP
        * @param model_mod 第一面顶吸基准点(0),其它面基准点(1)
        * @param agv_x AGV导航时距离前方距离
        * @param agv_y AGV导航时距离左侧距离
        * @param angle 倾角仪角度(AGV倾斜角)
        * @param j1_angle 装卸一体模式下,AGV拍照时一轴的关节角(单位：度，只有在装卸一体模式下才生效)
        * @param integrated_load_unload_mode 装卸一体模式(true: 开启, false: 摆台模式)
        * @param result 计算出的基准点坐标
        * @return 状态码
        */
        StatusCode processTradition(
            const CalibrationPose& calib_pose,
            const std::string& cameraIP,
            int model_mod,
            float agv_x,
            float agv_y,
            float angle,
            float j1_angle,
            bool integrated_load_unload_mode,
            Point3D& result);

        /**
        * @brief 机器人在最后一面侧吸基准点的计算
        * 
        * @param calib_pose 相机手眼标定外参位姿
        * @param cameraIP 相机IP
        * @param agv_x AGV导航时距离前方距离
        * @param agv_y AGV导航时距离左侧距离
        * @param j1_angle 装卸一体模式下,AGV拍照时一轴的关节角(单位：度，只有在装卸一体模式下才生效)
        * @param integrated_load_unload_mode 装卸一体模式(true: 开启, false: 摆台模式)
        * @param result 计算出的基准点坐标
        * @return 状态码
        */
        StatusCode processLastSurface(
            const CalibrationPose& calib_pose, 
            const std::string& cameraIP,
            float agv_x,
            float agv_y,
            float j1_angle,
            bool integrated_load_unload_mode,
            Point3D& result);

        /**
        * @brief 计算加强筋的法向,算出AGV航向角偏差
        * 
        * @param calib_pose 相机手眼标定外参位姿
        * @param cameraIP 相机IP
        * @param slam_x AGV导航时距离前方距离
        * @param slam_y AGV导航时距离左侧距离
        * @param j1_angle 装卸一体模式下,AGV拍照时一轴的关节角(单位：度，只有在装卸一体模式下才生效)
        * @param integrated_load_unload_mode 装卸一体模式(true: 开启, false: 摆台模式)
        * @param yaw 偏航角偏差 (度)，若计算失败或误差过大则返回 90.0
        * @return 状态码
        */
        StatusCode processYaw(
            const CalibrationPose& calib_pose,
            const std::string& cameraIP,
            float slam_x,
            float slam_y,
            float j1_angle,
            bool integrated_load_unload_mode,
            double& yaw);

    private:

        class Impl;
        Impl* impl_;
    };

    /**
     * @brief 将状态码转换为可读字符串
     */
    CAMERA3D_API std::string statusToString(StatusCode status);

}

#endif // CAMERA3D_SDK_H