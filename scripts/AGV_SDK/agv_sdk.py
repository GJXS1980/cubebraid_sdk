import os
import sys
import ctypes
from ctypes import Structure, c_double, c_float, c_int, c_bool, c_char_p, c_void_p
from dataclasses import dataclass
from enum import IntEnum

# ============================================================================
# 数据结构与枚举
# ============================================================================
class C_AGVPose(Structure):
    _fields_ = [
        ("x", c_double),
        ("y", c_double),
        ("z", c_double),
        ("roll", c_double),
        ("pitch", c_double),
        ("yaw", c_double)
    ]

@dataclass
class AGVPose:
    x: float = 0.0
    y: float = 0.0
    z: float = 0.0
    roll: float = 0.0
    pitch: float = 0.0
    yaw: float = 0.0

class ControlMode(IntEnum):
    MANUAL = 2
    AUTO = 3

# ============================================================================
# AGV SDK 核心封装类
# ============================================================================
class AGVClient:
    """AGV 底层 C-API 的 Python 封装类"""
    def __init__(self, lib_path: str = None):
        """
        初始化并加载动态链接库。
        :param lib_path: 动态库路径 (若未提供，将自动根据平台寻找同级目录下的 .dll 或 .so)
        """
        if lib_path is None:
            if sys.platform.startswith("win"):
                lib_path = os.path.abspath("../../bin/AGV_SDK.dll")
            else:
                lib_path = os.path.abspath("./libagv_sdk.so")

        if not os.path.exists(lib_path):
            raise FileNotFoundError(f"找不到动态链接库文件: {lib_path}")

        self._lib = ctypes.CDLL(lib_path)
        self._bind_functions()
        
        self._handle = self._lib.AGV_Create()
        if not self._handle:
            raise RuntimeError("无法创建 AGV 实例")

    def _bind_functions(self):
        """绑定 C 函数参数与返回值类型"""
        self._lib.AGV_Create.argtypes = []
        self._lib.AGV_Create.restype = c_void_p

        self._lib.AGV_Destroy.argtypes = [c_void_p]
        self._lib.AGV_Destroy.restype = None

        self._lib.AGV_Connect.argtypes = [c_void_p, c_char_p, c_int]
        self._lib.AGV_Connect.restype = c_bool

        self._lib.AGV_Disconnect.argtypes = [c_void_p]
        self._lib.AGV_Disconnect.restype = None

        self._lib.AGV_Login.argtypes = [c_void_p, c_char_p, c_char_p]
        self._lib.AGV_Login.restype = c_bool

        self._lib.AGV_Logout.argtypes = [c_void_p]
        self._lib.AGV_Logout.restype = c_bool

        self._lib.AGV_SwitchControlMode.argtypes = [c_void_p, c_int]
        self._lib.AGV_SwitchControlMode.restype = c_bool

        self._lib.AGV_GoForward.argtypes = [c_void_p, c_double, c_int]
        self._lib.AGV_GoForward.restype = c_bool

        self._lib.AGV_GoBack.argtypes = [c_void_p, c_double, c_int]
        self._lib.AGV_GoBack.restype = c_bool

        self._lib.AGV_ManualCtlVelSet.argtypes = [c_void_p, c_float, c_float, c_float]
        self._lib.AGV_ManualCtlVelSet.restype = c_bool

        self._lib.AGV_QuerySystemState.argtypes = [c_void_p]
        self._lib.AGV_QuerySystemState.restype = c_bool

        self._lib.AGV_SendHeartBeatsMsg.argtypes = [c_void_p]
        self._lib.AGV_SendHeartBeatsMsg.restype = c_bool

        self._lib.AGV_GetPose.argtypes = [c_void_p]
        self._lib.AGV_GetPose.restype = C_AGVPose

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()

    def close(self):
        """析构并释放 C++ 实例与网络连接"""
        if hasattr(self, "_handle") and self._handle:
            self.disconnect()
            self._lib.AGV_Destroy(self._handle)
            self._handle = None

    def __del__(self):
        self.close()

    # ------------------------------------------------------------------------
    # API 方法封装
    # ------------------------------------------------------------------------
    def connect(self, ip: str, port: int) -> bool:
        """连接 AGV 服务器"""
        return self._lib.AGV_Connect(self._handle, ip.encode('utf-8'), port)

    def disconnect(self):
        """断开连接"""
        if self._handle:
            self._lib.AGV_Disconnect(self._handle)

    def login(self, username: str, password_hash: str) -> bool:
        """登录设备"""
        return self._lib.AGV_Login(self._handle, username.encode('utf-8'), password_hash.encode('utf-8'))

    def logout(self) -> bool:
        """注销登录"""
        return self._lib.AGV_Logout(self._handle)

    def switch_control_mode(self, mode: ControlMode) -> bool:
        """切换控制模式 (MANUAL / AUTO)"""
        return self._lib.AGV_SwitchControlMode(self._handle, int(mode))

    def go_forward(self, dist_mm: float, timeout_ms: int = 30000) -> bool:
        """控制 AGV 前进 (阻塞至完成或超时)"""
        return self._lib.AGV_GoForward(self._handle, float(dist_mm), timeout_ms)

    def go_back(self, dist_mm: float, timeout_ms: int = 30000) -> bool:
        """控制 AGV 后退 (阻塞至完成或超时)"""
        return self._lib.AGV_GoBack(self._handle, float(dist_mm), timeout_ms)

    def set_manual_velocity(self, vx: float, vy: float, w: float) -> bool:
        """手动设置速度 (vx: mm/s, vy: mm/s, w: 0.001 rad/s)"""
        return self._lib.AGV_ManualCtlVelSet(self._handle, float(vx), float(vy), float(w))
    
    def move_manual_for_duration(self, vx: float, vy: float, w: float, duration_s: float, interval_s: float = 0.1) -> bool:
        """
        在指定时间内持续发送手动控制速度指令，并在到达时间后发送停止指令。

        :param vx: x方向线速度 (mm/s)
        :param vy: y方向线速度 (mm/s)
        :param w: 角速度 (0.001 rad/s)
        :param duration_s: 持续发送时间 (秒)
        :param interval_s: 速度指令重发间隔 (秒)，默认 100ms
        :return: bool 是否成功完成持续发送及停止
        """
        import time

        if duration_s <= 0:
            # 持续时间小于等于0，直接发送一次速度并返回
            return self.set_manual_velocity(vx, vy, w)

        end_time = time.monotonic() + duration_s
        success = True

        try:
            while time.monotonic() < end_time:
                # 持续发送速度心跳/控制包
                if not self.set_manual_velocity(vx, vy, w):
                    success = False
                    break
                
                # 计算下次发送的精准睡眠时间，避免延时累积
                remaining = end_time - time.monotonic()
                if remaining <= 0:
                    break
                time.sleep(min(interval_s, remaining))

        finally:
            # 无论中途是否出错，确保最终发送停止指令（速度归零）
            stop_ok = self.set_manual_velocity(0.0, 0.0, 0.0)
            if not stop_ok:
                success = False

        return success

    def query_system_state(self) -> bool:
        """发送系统状态查询请求"""
        return self._lib.AGV_QuerySystemState(self._handle)

    def send_heartbeat(self) -> bool:
        """发送心跳消息"""
        return self._lib.AGV_SendHeartBeatsMsg(self._handle)

    def get_pose(self) -> AGVPose:
        """获取当前最新位姿"""
        c_pose = self._lib.AGV_GetPose(self._handle)
        return AGVPose(
            x=c_pose.x,
            y=c_pose.y,
            z=c_pose.z,
            roll=c_pose.roll,
            pitch=c_pose.pitch,
            yaw=c_pose.yaw
        )