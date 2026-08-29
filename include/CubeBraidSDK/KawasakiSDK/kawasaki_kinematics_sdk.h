#ifndef KAWASAKI_KINEMATICS_SDK_H
#define KAWASAKI_KINEMATICS_SDK_H

// ============================================================
// DLL 导出宏
// ============================================================
// Windows:
//   编译 SDK DLL 时定义 PARAMETER_SDK_EXPORTS，导出函数。
//   使用 SDK DLL 时不定义，导入函数。
// Linux:
//   直接使用空定义。
// ============================================================
#ifdef _WIN32
    #ifdef PARAMETER_SDK_EXPORTS
        #define KAWASAKI_SDK_API __declspec(dllexport)
    #else
        #define KAWASAKI_SDK_API __declspec(dllimport)
    #endif
#else
    #define KAWASAKI_SDK_API
#endif

#include <vector>
#include <tuple>
#include <string>
#include <utility>
#include <Eigen/Dense>

namespace KawasakiRS080N 
{
    // 避障点结构体
    struct BypassPoint 
    {
        int step_index;         // 发生奇异点的插值步骤索引
        Eigen::VectorXd pose;   // 调整后的安全位姿 [x, y, z, z1, y, z2]
        std::string reason;     // 触发避障的原因
    };

    // 核心运动学求解类
    class KAWASAKI_SDK_API KinematicsSolver 
    {
    public:
        KinematicsSolver();
        ~KinematicsSolver() = default;

        // 逆运动学求解，返回有效关节角列表(度)
        std::vector<std::vector<double>> inverse_kinematics(const Eigen::Matrix4d& T06) const;
        
        // 正运动学计算，输入关节角度列表(度)，返回4x4齐次变换矩阵
        Eigen::Matrix4d forward_kinematics(const std::vector<double>& q_deg) const;
        
        // 检测并避开轨迹中的奇异点
        std::pair<std::vector<Eigen::VectorXd>, std::vector<BypassPoint>> 
        check_and_bypass_singularity(const Eigen::VectorXd& start_pose, 
                                     const Eigen::VectorXd& end_pose, 
                                     int steps, 
                                     bool forward_motion = true) const;

        // 提取指定索引的避障位姿
        static Eigen::VectorXd get_bypass_pose(const std::vector<BypassPoint>& bypass_points, size_t index);
        
        // 分解齐次变换矩阵：返回 (ZYZ欧拉角(度), 平移向量(mm))
        static std::tuple<Eigen::Vector3d, Eigen::Vector3d> decompose_homogeneous_matrix(const Eigen::Matrix4d& T);

    private:
        std::vector<double> alpha_rad;          // 连杆扭转角(弧度)
        std::vector<double> a;                  // 连杆长度(mm)
        std::vector<double> d;                  // 连杆偏移(mm)
        std::vector<double> theta_offset_rad;   // 关节偏移角(弧度)
        
        const std::vector<int> joint_sign = {-1, -1, 1, 1, 1, 1};
        // const std::vector<std::pair<double, double>> joint_limits = {
        //     {-150.0, 180.0}, {-70.0, 140.0}, {-155.0, 135.0},
        //     {-270.0, 270.0}, {-145.0, 145.0}, {-360.0, 360.0}
        // };
        // 关节角限位
        const std::vector<std::pair<double, double>> joint_limits = {
            {-150.0, 180.0}, {-70.0, 140.0}, {-155.0, 135.0},
            {-360.0, 360.0}, {-145.0, 145.0}, {-360.0, 360.0}
        };

        void initialize_parameters();
        bool within_limits(const std::vector<double>& q) const;
        std::vector<double> generate_angle_variants(double angle, double limit_min, double limit_max) const;
        std::vector<std::tuple<double, double, double>> solve_theta4_to_theta6(const Eigen::Matrix3d& R36) const;
        Eigen::Matrix4d dh_transform(int i, double theta) const;
        std::vector<Eigen::VectorXd> interpolate_pose(const Eigen::VectorXd& start_pose, const Eigen::VectorXd& end_pose, int steps) const;
        Eigen::Matrix4d pose_to_T(const Eigen::VectorXd& pose) const;
        bool has_singularity(const std::vector<std::vector<double>>& solutions) const;
        Eigen::VectorXd find_non_singular_alternative(const Eigen::VectorXd& pose, int max_attempts, 
                                                     double offset_mm, double max_x_offset, bool forward_motion) const;
    };
}

// ============================================================
// C 接口
// ============================================================
// 使用 extern "C" 防止 C++ 名称修饰，便于：
// 1. C++ 项目静态/动态链接
// 2. Python ctypes 零依赖直接调用
// 3. 其他语言 (C#, Rust, LabVIEW等) 动态加载 DLL/SO
// ============================================================

#ifdef __cplusplus
extern "C" 
{
    #endif

    // 创建运动学求解器句柄
    KAWASAKI_SDK_API
    void* create_kinematics_solver();

    // 销毁运动学求解器句柄
    KAWASAKI_SDK_API
    void destroy_kinematics_solver(void* handle);

    // 正运动学计算
    // q_deg: 传入6个元素的关节角数组 (单位: 度)
    // out_T16: 传出16个元素的平铺4x4矩阵数组
    KAWASAKI_SDK_API
    void fk_calculator(void* handle, const double* q_deg, double* out_T16);

    // 逆运动学计算
    // T16: 传入16个元素的平铺4x4矩阵数组
    // out_solutions: 传出解集的数组 buffer (建议预分配空间: double[8][6])
    // 返回值: 实际计算出来的有效解数量 (0 ~ 8)
    KAWASAKI_SDK_API
    int ik_calculator(void* handle, const double* T16, double* out_solutions);

    #ifdef __cplusplus
}
#endif

#endif // KAWASAKI_KINEMATICS_SDK_H