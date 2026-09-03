import os
import sys
import ctypes
import time
from ctypes import Structure, c_double, c_float, c_int, c_bool, c_char_p, c_void_p, POINTER, byref

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

# 机器人位姿 (XYZABC,位置（单位：毫米）,姿态（单位：°）)
class Pose(Structure):
    _fields_ = [
        ("x", c_double), ("y", c_double), ("z", c_double),
        ("rx", c_double), ("ry", c_double), ("rz", c_double)
    ]

# 箱体/SKU 尺寸结构体 (mm)
class BoxDimension(Structure):
    _fields_ = [
        ("length", c_float), ("width", c_float), ("height", c_float)
    ]

# ============================================================
# 声明 C API 函数参数类型与返回值类型
# ============================================================
# 创建机器人控制器实例
# RobotHandle Robot_Create();
sdk.Robot_Create.restype = c_void_p
sdk.Robot_Create.argtypes = []

# 销毁机器人控制器实例
# void Robot_Destroy(RobotHandle handle);
sdk.Robot_Destroy.restype = None
sdk.Robot_Destroy.argtypes = [c_void_p]

# 连接机器人
# int Robot_Connect(RobotHandle handle, const char* ip, int motionPort, int statusPort);
sdk.Robot_Connect.restype = c_int
sdk.Robot_Connect.argtypes = [c_void_p, c_char_p, c_int, c_int]

# 断开连接
# void Robot_Disconnect(RobotHandle handle);
sdk.Robot_Disconnect.restype = None
sdk.Robot_Disconnect.argtypes = [c_void_p]

# 笛卡尔空间位姿控制
# int Robot_ControlPosture(RobotHandle handle, int mode, double x, double y, double z, double rx, double ry, double rz);
sdk.Robot_ControlPosture.restype = c_int
sdk.Robot_ControlPosture.argtypes = [c_void_p, c_int, c_double, c_double, c_double, c_double, c_double, c_double]

# 关节角度控制
# int Robot_ControlJoint(RobotHandle handle, int mode, double j1, double j2, double j3, double j4, double j5, double j6);
sdk.Robot_ControlJoint.restype = c_int
sdk.Robot_ControlJoint.argtypes = [c_void_p, c_int, c_double, c_double, c_double, c_double, c_double, c_double]

# 获取当前位姿
# int Robot_GetCurrentPose(RobotHandle handle, double* x, double* y, double* z, double* rx, double* ry, double* rz);
sdk.Robot_GetCurrentPose.restype = c_int
sdk.Robot_GetCurrentPose.argtypes = [c_void_p, POINTER(c_double), POINTER(c_double), POINTER(c_double), POINTER(c_double), POINTER(c_double), POINTER(c_double)]

# 获取当前J1-J6关节角度
# int Robot_GetCurrentJoint(RobotHandle handle, double* j1, double* j2, double* j3, double* j4, double* j5, double* j6);
sdk.Robot_GetCurrentJoint.restype = c_int
sdk.Robot_GetCurrentJoint.argtypes = [c_void_p, POINTER(c_double), POINTER(c_double), POINTER(c_double), POINTER(c_double), POINTER(c_double), POINTER(c_double)]

# 获取 J4 关节角度
# double Robot_GetJoint4Angle(RobotHandle handle, double* j4);
sdk.Robot_GetJoint4Angle.restype = c_int
sdk.Robot_GetJoint4Angle.argtypes = [c_void_p, POINTER(c_double)]

# 带倾角补偿的顶吸目标位姿计算
# int Robot_TopSuctionAngle(RobotHandle handle, robot_sdk::Pose centroid, robot_sdk::BoxDimension box, int fetchMode, int sku_num, float dis_y, 
                            # robot_sdk::Pose poseOffset, bool ROffset, int model_mod, double inclx_angle, double* x, double* y, double* z);
sdk.Robot_TopSuctionAngle.restype = c_int
sdk.Robot_TopSuctionAngle.argtypes = [
    c_void_p,                                   # handle
    Pose, BoxDimension, c_int,                  # centroid, box, fetchMode
    c_int, c_float, Pose, c_bool,               # sku_num, dis_y, poseOffset, ROffset
    c_int, c_double,                            # model_mod, inclx_angle
    POINTER(c_double), POINTER(c_double), POINTER(c_double) # out x, y, z
]

# 带倾角补偿的顶吸目标位姿计算(特殊码法)
# int Robot_TopSuctionSpecial(RobotHandle handle, robot_sdk::Pose centroid, robot_sdk::BoxDimension box, int fetchMode, int sku_num, float dis_y, 
                            # robot_sdk::Pose poseOffset, bool ROffset, int model_mod, double inclx_angle, double* x, double* y, double* z);
sdk.Robot_TopSuctionSpecial.restype = c_int
sdk.Robot_TopSuctionSpecial.argtypes = [
    c_void_p,                                   # handle
    Pose, BoxDimension, c_int,                  # centroid, box, fetchMode
    c_int, c_float, Pose, c_bool,               # sku_num, dis_y, poseOffset, ROffset
    c_int, c_double,                            # model_mod, inclx_angle
    POINTER(c_double), POINTER(c_double), POINTER(c_double) # out x, y, z
]

# 带倾角补偿的侧吸目标位姿计算
sdk.Robot_SideSuctionAngle.restype = c_int
sdk.Robot_SideSuctionAngle.argtypes = [
    c_void_p,                                   # handle
    Pose, BoxDimension, c_int, c_int,                 # centroid, box, fetchMode, fetchMode_side
    c_int, c_float, Pose, c_bool,               # sku_num, dis_y, poseOffset, ROffset
    c_int, c_double, c_float, c_bool,           # model_mod, inclx_angle, container_h, switch_top_bottom_suction
    POINTER(c_double), POINTER(c_double), POINTER(c_double) # out x, y, z
]

# ============================================================
# 业务逻辑控制封装类 (Pythonic Wrapper)
# ============================================================
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

    def get_current_pose(self) -> tuple:
        """
        获取当前机器人的笛卡尔位姿 (x, y, z, rx, ry, rz)
        :return: (x, y, z, rx, ry, rz) 元组
        """
        x, y, z = c_double(), c_double(), c_double()
        rx, ry, rz = c_double(), c_double(), c_double()
        ret = sdk.Robot_GetCurrentPose(
            self._handle, 
            byref(x), byref(y), byref(z), 
            byref(rx), byref(ry), byref(rz)
        )
        if ret == 1:
            return (x.value, y.value, z.value, rx.value, ry.value, rz.value)
        return (0.0, 0.0, 0.0, 0.0, 0.0, 0.0)

    def get_current_joint(self) -> tuple:
        """
        获取当前机器人的关节角度 (j1, j2, j3, j4, j5, j6)
        :return: (j1, j2, j3, j4, j5, j6) 元组
        """
        j1, j2, j3 = c_double(), c_double(), c_double()
        j4, j5, j6 = c_double(), c_double(), c_double()
        ret = sdk.Robot_GetCurrentJoint(
            self._handle, 
            byref(j1), byref(j2), byref(j3), 
            byref(j4), byref(j5), byref(j6)
        )
        if ret == 1:
            return (j1.value, j2.value, j3.value, j4.value, j5.value, j6.value)
        return (0.0, 0.0, 0.0, 0.0, 0.0, 0.0)
    
    def get_joint4_angle(self) -> float:
        """
        获取 J4 轴单个关节角度
        :return: J4 角度值 (失败时返回 0.0)
        """
        j4 = c_double(0.0)
        ret = sdk.Robot_GetJoint4Angle(self._handle, byref(j4))
        if ret == 1:
            return j4.value
        return 0.0
    
    def top_suction_angle(self, centroid: Pose, box: BoxDimension, fetch_mode: int, sku_num: int, dis_y: float, pose_offset: Pose, r_offset: bool,
                            model_mod: int, inclx_angle: float) -> tuple:
        """
        带倾角补偿的顶吸目标位姿计算
        :return: 装柜目标位置 (x, y, z)
        """
        x, y, z = c_double(), c_double(), c_double()
        ret = sdk.Robot_TopSuctionAngle(
            self._handle,
            centroid, box, fetch_mode,
            sku_num, c_float(dis_y), pose_offset, r_offset,
            model_mod, c_double(inclx_angle),
            byref(x), byref(y), byref(z)
        )
        if ret == 1:
            return (x.value, y.value, z.value)
        return (0.0, 0.0, 0.0)
        
    def top_suction_special(self, centroid: Pose, box: BoxDimension, fetch_mode: int, sku_num: int, dis_y: float, pose_offset: Pose, r_offset: bool,
                            model_mod: int, inclx_angle: float) -> tuple:
        """
        带倾角补偿的顶吸目标位姿计算(特殊码法)
        :return: 装柜目标位置 (x, y, z)
        """
        x, y, z = c_double(), c_double(), c_double()
        ret = sdk.Robot_TopSuctionSpecial(
            self._handle,
            centroid, box, fetch_mode,
            sku_num, c_float(dis_y), pose_offset, r_offset,
            model_mod, c_double(inclx_angle),
            byref(x), byref(y), byref(z)
        )
        if ret == 1:
            return (x.value, y.value, z.value)
        return (0.0, 0.0, 0.0)
    def side_suction_angle(self, centroid: Pose, box: BoxDimension, fetch_mode: int, fetch_mode_side: int, sku_num: int, dis_y: float, pose_offset: Pose, r_offset: bool,
                                model_mod: int, inclx_angle: float, container_h: float, switch_top_bottom_suction: bool) -> tuple:
        """
        带倾角补偿的侧吸目标位姿计算
        :return: 装柜目标位置 (x, y, z)
        """
        x, y, z = c_double(), c_double(), c_double()
        ret = sdk.Robot_SideSuctionAngle(
            self._handle,
            centroid, box, fetch_mode, fetch_mode_side,
            sku_num, c_float(dis_y), pose_offset, r_offset,
            model_mod, c_double(inclx_angle), c_float(container_h), switch_top_bottom_suction,
            byref(x), byref(y), byref(z)
        )
        if ret == 1:
            return (x.value, y.value, z.value)
        return (0.0, 0.0, 0.0)