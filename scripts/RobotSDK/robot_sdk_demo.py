from robot_sdk import RobotController, Pose, BoxDimension
import time

# ============================================================
# 主程序运行 Demo
# ============================================================
if __name__ == "__main__":
    robot_ip = "192.168.0.2"  # 替换为实际机器人 IP
    
    print("正在初始化 SDK 并在 Python 中调用...")
    robot = RobotController()

    print(f"尝试连接机械臂 ({robot_ip})...")
    if robot.connect(robot_ip):
        print("连接机械臂成功！")
        
        time.sleep(5)   # 延时5s

        # ------------------------------------------------------------
        # 查询初始位姿与关节角
        # ------------------------------------------------------------
        pose = robot.get_current_pose()
        joint = robot.get_current_joint()
        j4_angle = robot.get_joint4_angle()

        print(f"\n[初始状态] 当前位姿 (X, Y, Z, RX, RY, RZ): "
              f"{pose[0]:.2f}, {pose[1]:.2f}, {pose[2]:.2f}, {pose[3]:.2f}, {pose[4]:.2f}, {pose[5]:.2f}")
        print(f"[初始状态] 当前关节角 (J1-J6): "
              f"{joint[0]:.2f}, {joint[1]:.2f}, {joint[2]:.2f}, {joint[3]:.2f}, {joint[4]:.2f}, {joint[5]:.2f}")
        print(f"[初始状态] J4 关节单独角度: {j4_angle:.2f}°\n")

        # ------------------------------------------------------------
        # 示例 1: 关节空间运动 (前往粗略过渡点)
        # ------------------------------------------------------------
        print("发送关节角度指令...")
        success = robot.control_joint(
            mode=1, 
            j1=-90.0, j2=-13.73, j3=-96.28, j4=20.0, j5=-97.45, j6=0.0
        )
        while not success:
            time.sleep(0.2)
            print("关节控制指令发送失败,重新发送!")
            success = robot.control_joint(mode=1, j1=-90.0, j2=-13.73, j3=-96.28, j4=20.0, j5=-97.45, j6=0.0)
        
        print(f"关节控制指令执行成功!")

        # 查询运动后的关节角
        time.sleep(5)   # 延时5s
        curr_joint = robot.get_current_joint()
        j4_angle = robot.get_joint4_angle()
        print(f"[更新状态] 实际关节角: {curr_joint[0]:.2f}, {curr_joint[1]:.2f}, {curr_joint[2]:.2f}, "
              f"{curr_joint[3]:.2f}, {curr_joint[4]:.2f}, {curr_joint[5]:.2f}\n")
        print(f"[初始状态] J4 关节单独角度: {j4_angle:.2f}°\n")

        # ------------------------------------------------------------
        # 示例 2: 笛卡尔空间位姿运动 (前往目标精细抓取点)
        # ------------------------------------------------------------
        print("发送笛卡尔位姿指令...")
        target_x, target_y, target_z = 50, 1014.32, 1500.16
        target_rx, target_ry, target_rz = -82.69, 180.0, 7.31
        
        success = robot.control_posture(
            mode=2, 
            x=target_x, y=target_y, z=target_z, 
            rx=target_rx, ry=target_ry, rz=target_rz
        )
        while not success:
            time.sleep(0.2)
            print("位姿控制指令发送失败,重新发送!")
            success = robot.control_posture(mode=2, x=target_x, y=target_y, z=target_z, rx=target_rx, ry=target_ry, rz=target_rz)
        
        print(f"位姿控制指令执行成功!")

        # 查询运动后的实际位姿
        time.sleep(5)   # 延时5s
        curr_pose = robot.get_current_pose()
        j4_angle = robot.get_joint4_angle()
        print(f"[更新状态] 实际位姿: {curr_pose[0]:.2f}, {curr_pose[1]:.2f}, {curr_pose[2]:.2f}, "
              f"{curr_pose[3]:.2f}, {curr_pose[4]:.2f}, {curr_pose[5]:.2f}\n")
        print(f"[初始状态] J4 关节单独角度: {j4_angle:.2f}°\n")

        # 断开连接
        robot.disconnect()
        print("已断开与机械臂的连接。")
    else:
        print("连接机械臂失败，请检查网络设置或机器人控制器状态。")
        
    try:
        # 构造测试数据
        # 基准点坐标 (单位: 米 m)
        centroid = Pose(x=1.46328, y=1.52448, z=-0.611929, rx=0.0, ry=0.0, rz=0.0)
        
        # 箱体尺寸 (单位: 毫米 mm) -> 长、宽、高
        box = BoxDimension(length=570.0, width=453.0, height=330.0)
        
        # 垛型 JSON 偏移量 (单位: 毫米 mm)
        pose_offset = Pose(x=-570.0, y=-453.0, z=330.0, rx=0.0, ry=0.0, rz=0.0)
        
        # 其他算法入参
        fetch_mode = 3      # 抓取模式: 3 或 4 代表沿短边抓取
        sku_num = 1        # 并排抓取 SKU 数量
        dis_y = 75.0        # 每层剩余缝隙 (mm)
        r_offset = True     # 垛型最左侧标志位
        model_mod = 0       # 0: 第一面, 1: 其它面
        inclx_angle = 0.0  # 倾角仪角度 (<= 0.15°)

        # 调用特殊码法顶吸目标位姿计算接口
        x, y, z = robot.top_suction_angle(
            centroid=centroid,
            box=box,
            fetch_mode=fetch_mode,
            sku_num=sku_num,
            dis_y=dis_y,
            pose_offset=pose_offset,
            r_offset=r_offset,
            model_mod=model_mod,
            inclx_angle=inclx_angle
        )

        # 打印计算得到的 XYZ 坐标 (单位: 毫米 mm)
        print("====== Top Suction Angle 计算结果 ======")
        print(f"Target Pose -> X: {x:.3f} mm, Y: {y:.3f} mm, Z: {z:.3f} mm")
        
        # 调用特殊码法顶吸目标位姿计算接口
        x, y, z = robot.top_suction_special(
            centroid=centroid,
            box=box,
            fetch_mode=fetch_mode,
            sku_num=sku_num,
            dis_y=dis_y,
            pose_offset=pose_offset,
            r_offset=r_offset,
            model_mod=model_mod,
            inclx_angle=inclx_angle
        )

        # 打印计算得到的 XYZ 坐标 (单位: 毫米 mm)
        print("====== Top Suction Special 计算结果 ======")
        print(f"Target Pose -> X: {x:.3f} mm, Y: {y:.3f} mm, Z: {z:.3f} mm")
        
        # 调用上层侧吸目标位姿计算接口
        fetch_mode_side = 3      # 抓取模式: 3 或 4 代表沿短边抓取
        container_h = 2698      # 集装箱高度
        switch_top_bottom_suction = False      # 上层侧吸
        x, y, z = robot.side_suction_angle(
            centroid=centroid,
            box=box,
            fetch_mode=fetch_mode,
            fetch_mode_side=fetch_mode_side,
            sku_num=sku_num,
            dis_y=dis_y,
            pose_offset=pose_offset,
            r_offset=r_offset,
            model_mod=model_mod,
            inclx_angle=inclx_angle,
            container_h=container_h,
            switch_top_bottom_suction=switch_top_bottom_suction
        )

        # 打印计算得到的 XYZ 坐标 (单位: 毫米 mm)
        print("====== Side Suction Angle 计算结果 ======")
        print(f"Target Pose -> X: {x:.3f} mm, Y: {y:.3f} mm, Z: {z:.3f} mm")
        
        # 调用下层侧吸目标位姿计算接口
        fetch_mode_side = 3      # 抓取模式: 3 或 4 代表沿短边抓取
        container_h = 2698      # 集装箱高度
        switch_top_bottom_suction = True      # 下层侧吸
        x, y, z = robot.side_suction_angle(
            centroid=centroid,
            box=box,
            fetch_mode=fetch_mode,
            fetch_mode_side=fetch_mode_side,
            sku_num=sku_num,
            dis_y=dis_y,
            pose_offset=pose_offset,
            r_offset=r_offset,
            model_mod=model_mod,
            inclx_angle=inclx_angle,
            container_h=container_h,
            switch_top_bottom_suction=switch_top_bottom_suction
        )

        # 打印计算得到的 XYZ 坐标 (单位: 毫米 mm)
        print("====== Side Suction Angle 计算结果 ======")
        print(f"Target Pose -> X: {x:.3f} mm, Y: {y:.3f} mm, Z: {z:.3f} mm")
        
        # 准备角度输入数据 (Z, Y, Z),单位：度
        euler_before = (10.0, 20.0, 30.0)
        euler_temp = (5.0, -10.0, 15.0)

        # 计算相对旋转角度
        delta_deg = robot.compute_delta_euler_zyz_deg(euler_before, euler_temp)

        print("====== ZYZ 欧拉角相对旋转计算结果 ======")
        print(f"Delta Z1: {delta_deg[0]:.4f}°, Delta Y: {delta_deg[1]:.4f}°, Delta Z2: {delta_deg[2]:.4f}°")
        

    except Exception as e:
        print(f"执行异常: {e}")
        
    finally:
        # 对象销毁时会自动触发 __del__ 内部的 Robot_Destroy，无需显式释放
        del robot