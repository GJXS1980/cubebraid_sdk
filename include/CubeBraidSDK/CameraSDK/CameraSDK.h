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
    #define CAMERA3D_API
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
     * @brief 相机配置
     */
    struct CameraConfig
    {
        std::string camera_ip = "192.168.23.203";
        std::string camera_ip_up = "192.168.23.88";

        std::string depth_file = "./data/img/depth.tiff";
        std::string color_file = "./data/img/demo.png";

        std::string corner_model_path =
            "./data/models/corner_model.onnx";

        std::string box_model_path =
            "./data/models/box_model.onnx";
    };

    /**
     * @brief Camera3D SDK主类
     */
    class CAMERA3D_API Camera3D
    {
    public:

        Camera3D();
        ~Camera3D();

        StatusCode initialize(
            const CameraConfig& config);

        bool connect();

        void disconnect();

        bool isConnected() const;

        std::string getLastError() const;

        StatusCode getLastStatus() const;

        /**
         * @brief 普通基准点计算
         */
        StatusCode processTradition(
            int cam_cam_mod,
            int model_mod,
            float agv_x,
            float agv_y,
            Point3D& result);

        /**
         * @brief 坡度场景计算
         */
        StatusCode processSlope(
            int cam_cam_mod,
            int model_mod,
            float agv_x,
            float agv_y,
            float angle,
            Point3D& result);

        /**
         * @brief 最后一面计算
         */
        StatusCode processLastSurface(
            int cam_cam_mod,
            int model_mod,
            float agv_x,
            float agv_y,
            Point3D& result);

        /**
         * @brief 计算Yaw
         */
        StatusCode processYaw(
            int cam_cam_mod,
            float slam_x,
            float slam_y,
            double& yaw);

        /**
         * @brief 设置机器人4×4变换矩阵
         */
        StatusCode setRobotTransform(
            const double matrix[16]);

        /**
         * @brief 设置相机模式
         */
        void setCameraMode(int mode);

        int getCameraMode() const;

        void setDistance(double distance);

        void setDepthDistance(double distance);

    private:

        class Impl;

        Impl* impl_;
    };

    CAMERA3D_API std::string statusToString(
        StatusCode status);

}

#endif // CAMERA3D_SDK_H