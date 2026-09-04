import time
from agv_sdk import AGVClient, ControlMode

def main():
    # 配置信息
    IP = "192.168.1.91"
    PORT = 5005
    USER = "admin"
    PASS_HASH = "21232f297a57a5a743894a0e4a801fc3"

    # 使用 with 上下文管理器，确保退出程序时自动调用 disconnect 和 destroy 释放内存
    try:
        with AGVClient() as agv:
            print(f"正在连接 AGV [{IP}:{PORT}]...")
            if not agv.connect(IP, PORT):
                print("连接失败！")
                return
            print("连接成功。")

            print("执行登录...")
            if not agv.login(USER, PASS_HASH):
                print("登录失败！")
                return
            print("登录成功。")
            time.sleep(1)

            # 切换自动模式并走位
            print("切换至【自动模式】...")
            agv.switch_control_mode(ControlMode.AUTO)

            print("指令：前进到底1800mm...")
            if agv.go_forward(1800.0, timeout_ms=100000):
                print("前进到达指定位置")
            else:
                print("前进超时或未成功完成")

            # 读取位姿
            pose = agv.get_pose()
            print(f"当前位姿 -> X: {pose.x:.3f}m, Y: {pose.y:.3f}m, Yaw: {pose.yaw:.2f}°")

            print("指令：单步前进500mm...")
            if agv.go_back(500.0, timeout_ms=100000):
                print("前进到达指定位置")
            else:
                print("前进超时或未成功完成")

            # 读取位姿
            pose = agv.get_pose()
            print(f"当前位姿 -> X: {pose.x:.3f}m, Y: {pose.y:.3f}m, Yaw: {pose.yaw:.2f}°")

            print("指令：单步后退500mm...")
            if agv.go_back(-500.0, timeout_ms=100000):
                print("前进到达指定位置")
            else:
                print("前进超时或未成功完成")

            # 读取位姿
            pose = agv.get_pose()
            print(f"当前位姿 -> X: {pose.x:.3f}m, Y: {pose.y:.3f}m, Yaw: {pose.yaw:.2f}°")

            # 切换手动模式运动控制
            print("切换至【手动模式】...")
            agv.switch_control_mode(ControlMode.MANUAL)

            # 示例：以 vx=100 mm/s 前进，持续移动 2 秒（内部会自动以 100ms 频率持续发包，到期后自动归零停止）
            print("开始手动移动 2 秒...")
            if agv.move_manual_for_duration(vx=100.0, vy=0.0, w=0.0, duration_s=2, interval_s=0.1):
                print("手动移动完成并已安全停止")
            else:
                print("移动过程中出现异常")

            # 示例：以 vx=-100 mm/s 前进，持续移动 2 秒（内部会自动以 100ms 频率持续发包，到期后自动归零停止）
            print("开始手动移动 2 秒...")
            if agv.move_manual_for_duration(vx=-100.0, vy=0.0, w=0.0, duration_s=2, interval_s=0.1):
                print("手动移动完成并已安全停止")
            else:
                print("移动过程中出现异常")


            # 示例：以 w=0.1rad/s 前进，持续移动 2 秒（内部会自动以 100ms 频率持续发包，到期后自动归零停止）
            print("开始手动移动 2 秒...")
            if agv.move_manual_for_duration(vx=0.0, vy=0.0, w=100.0, duration_s=2, interval_s=0.1):
                print("手动移动完成并已安全停止")
            else:
                print("移动过程中出现异常")

            # 示例：以 w=-0.1rad/s 前进，持续移动 2 秒（内部会自动以 100ms 频率持续发包，到期后自动归零停止）
            print("开始手动移动 2 秒...")
            if agv.move_manual_for_duration(vx=0.0, vy=0.0, w=-100.0, duration_s=2, interval_s=0.1):
                print("手动移动完成并已安全停止")
            else:
                print("移动过程中出现异常")

            # 安全登出
            agv.logout()
            print("已安全登出。")

    except Exception as e:
        print(f"运行时发生错误: {e}")

if __name__ == "__main__":
    main()