from robot_sdk import RobotController
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