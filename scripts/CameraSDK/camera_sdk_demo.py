from camera_sdk import Camera3DSDK, Camera3DCalibrationPose

def main():
    print("=== Camera3D Python SDK Demo 程序启动 ===")

    # 定义测试用的相机 IP
    target_camera_ip = "192.168.23.203"
    target_camera_ip_up = "192.168.23.88"

    # 配置手眼标定外参位姿 (相机在机器人机械臂/车体上的相对姿态)
    calib_pose = Camera3DCalibrationPose(
        x=0.12, y=-0.05, z=0.85,
        qw=1.0, qx=0.0, qy=0.0, qz=0.0
    )

    # 使用 contextmanager (with 语句) 管理 SDK 上下文生命周期，防止内存泄漏
    try:
        with Camera3DSDK() as camera:
            # --------------------------------------------------------
            # 算法测试 1: 集装箱内部/斜坡基准点计算
            # --------------------------------------------------------
            pt_tradition = camera.process_tradition(
                pose=calib_pose,
                camera_ip=target_camera_ip,  # 补全 camera_ip 参数
                model_mod=0,
                agv_x=1.25,
                agv_y=0.85,
                angle=0.3,
                j1_angle=15.0,
                integrated_mode=False
            )
            print(f"[成功] 传统基准点坐标: {pt_tradition}")

            # --------------------------------------------------------
            # 算法测试 2: 最后一面侧吸基准点计算
            # --------------------------------------------------------
            pt_last = camera.process_last_surface(
                pose=calib_pose,
                camera_ip=target_camera_ip,  # 补全 camera_ip 参数
                agv_x=1.25,
                agv_y=0.85,
                j1_angle=15.0,
                integrated_mode=False
            )
            print(f"[成功] 最后一面基准点坐标: {pt_last}")

            # --------------------------------------------------------
            # 算法测试 3: AGV 航向角偏差计算
            # --------------------------------------------------------
            yaw_deg = camera.process_yaw(
                pose=calib_pose,
                camera_ip=target_camera_ip_up,  # 补全 target_camera_ip_up 参数
                slam_x=1.25,
                slam_y=0.85,
                j1_angle=15.0,
                integrated_mode=False
            )
            print(f"[成功] 航向角偏差 (Yaw): {yaw_deg:.4f} 度")
       

    except RuntimeError as e:
        print(f"[运行时错误] {e}")
    except Exception as e:
        print(f"[未知异常] {e}")

    print("=== Demo 退出 ===")

if __name__ == "__main__":
    main()