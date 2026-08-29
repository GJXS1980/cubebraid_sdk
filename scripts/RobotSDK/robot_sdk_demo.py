import os
import sys
import ctypes
import time
from ctypes import Structure, c_double, c_float, c_int, c_bool, c_char_p, c_void_p

# 加载 C++ SDK 动态库
# 获取当前脚本所在目录
lib_path = os.path.dirname(os.path.abspath(__file__))

if sys.platform.startswith("win"):
    lib_name = os.path.join(lib_path, "../../bin/RobotSDK.dll")
else:
    lib_name = os.path.join(lib_path, "../../bin/libRobotSDK.so")

try:
    sdk = ctypes.CDLL(lib_name)
except OSError as e:
    print(f"动态库加载失败，请检查路径及依赖: {e}")
    sys.exit(1)

# 声明 C API 函数参数类型与返回值类型
# RobotHandle Robot_Create();
sdk.Robot_Create.restype = c_void_p
sdk.Robot_Create.argtypes = []

# void Robot_Destroy(RobotHandle handle);
sdk.Robot_Destroy.restype = None
sdk.Robot_Destroy.argtypes = [c_void_p]

# int Robot_Connect(RobotHandle handle, const char* ip, int motionPort, int statusPort);
sdk.Robot_Connect.restype = c_int
sdk.Robot_Connect.argtypes = [c_void_p, c_char_p, c_int, c_int]

# void Robot_Disconnect(RobotHandle handle);
sdk.Robot_Disconnect.restype = None
sdk.Robot_Disconnect.argtypes = [c_void_p]

# int Robot_ControlPosture(RobotHandle handle, int mode, double x, double y, double z, double rx, double ry, double rz);
sdk.Robot_ControlPosture.restype = c_int
sdk.Robot_ControlPosture.argtypes = [c_void_p, c_int, c_double, c_double, c_double, c_double, c_double, c_double]

# int Robot_ControlJoint(RobotHandle handle, int mode, double j1, double j2, double j3, double j4, double j5, double j6);
sdk.Robot_ControlJoint.restype = c_int
sdk.Robot_ControlJoint.argtypes = [c_void_p, c_int, c_double, c_double, c_double, c_double, c_double, c_double]

# 业务逻辑控制封装类 (Pythonic Wrapper)
class RobotController:
    def __init__(self):
        # 创建 SDK 实例句柄
        self._handle = sdk.Robot_Create()
        if not self._handle:
            raise RuntimeError("创建机器人控制器实例失败!")

    def __del__(self):
        # 析构时安全释放底层句柄
        if hasattr(self, '_handle') and self._handle:
            sdk.Robot_Destroy(self._handle)
            self._handle = None

    def connect(self, ip: str, motion_port: int = 31400, status_port: int = 31401) -> bool:
        """连接机器人"""
        ip_bytes = ip.encode('utf-8')
        ret = sdk.Robot_Connect(self._handle, ip_bytes, motion_port, status_port)
        return ret == 1

    def disconnect(self):
        """断开机器人连接"""
        sdk.Robot_Disconnect(self._handle)

    def control_posture(self, mode: int, x: float, y: float, z: float, rx: float, ry: float, rz: float) -> bool:
        """
        位姿控制
        :param mode: 1-粗略过渡点(50mm), 2-精准目标点(1mm), 3-远距离过渡点(200mm)
        """
        ret = sdk.Robot_ControlPosture(
            self._handle, mode, 
            c_double(x), c_double(y), c_double(z), 
            c_double(rx), c_double(ry), c_double(rz)
        )
        return ret == 1

    def control_joint(self, mode: int, j1: float, j2: float, j3: float, j4: float, j5: float, j6: float) -> bool:
        """
        关节控制
        :param mode: 1-粗略过渡点, 2-精准目标点
        """
        ret = sdk.Robot_ControlJoint(
            self._handle, mode, 
            c_double(j1), c_double(j2), c_double(j3), 
            c_double(j4), c_double(j5), c_double(j6)
        )
        return ret == 1

# 主程序运行 Demo
if __name__ == "__main__":
    robot_ip = "192.168.0.2"  # 替换为实际机器人 IP
    
    print("正在初始化 SDK 并在 Python 中调用...")
    robot = RobotController()

    print(f"尝试连接机械臂 ({robot_ip})...")
    # 模拟连接测试
    if robot.connect(robot_ip):
        print("连接机械臂成功！")

        # 示例 1: 关节空间运动 (前往粗略过渡点)
        print("发送关节角度指令...")
        success = robot.control_joint(
            mode=1, 
            j1=-85.0, j2=-13.73, j3=-96.28, j4=0.0, j5=-97.45, j6=0.0
        )
        while(success == False):
            time.sleep(1)
            print("关节控制指令发送失败,重新发送!")
            success = robot.control_joint( mode=1, j1=-85.0, j2=-13.73, j3=-96.28, j4=0.0, j5=-97.45, j6=0.0)
        print(f"关节控制指令执行结果: {success}")

        # 示例 2: 笛卡尔空间位姿运动 (前往目标精细抓取点)
        print("发送笛卡尔位姿指令...")
        target_x, target_y, target_z = -0.03, 1014.32, 1500.16
        target_rx, target_ry, target_rz = -82.69, 180.0, 7.31
        
        success = robot.control_posture(
            mode=2, 
            x=target_x, y=target_y, z=target_z, 
            rx=target_rx, ry=target_ry, rz=target_rz
        )
        while(success == False):
            time.sleep(1)
            print("位姿控制指令发送失败,重新发送!")
            success = robot.control_posture(mode=2, x=target_x, y=target_y, z=target_z, rx=target_rx, ry=target_ry, rz=target_rz)
        print(f"位姿控制指令执行结果: {success}")

        # 断开连接
        robot.disconnect()
        print("已断开与机械臂的连接。")
    else:
        print("连接机械臂失败，请检查网络设置或机器人控制器状态。")