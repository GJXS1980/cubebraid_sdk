import os
import sys
import ctypes

from ctypes import (
    Structure, c_void_p, c_int, c_float, c_double, c_char_p,
    POINTER, CDLL, create_string_buffer, byref
)


# ============================================================
# C 兼容结构体映射
# ============================================================

class Camera3DPoint(Structure):
    """三维点结构"""
    _fields_ = [
        ("x", c_float),
        ("y", c_float),
        ("z", c_float)
    ]

    def __repr__(self):
        return f"Point3D(x={self.x:.3f}, y={self.y:.3f}, z={self.z:.3f})"


class Camera3DCalibrationPose(Structure):
    """相机手眼标定外参位姿"""
    _fields_ = [
        ("x", c_float),
        ("y", c_float),
        ("z", c_float),
        ("qw", c_float),
        ("qx", c_float),
        ("qy", c_float),
        ("qz", c_float)
    ]


class Camera3DConfig(Structure):
    """相机配置"""
    _fields_ = [
        ("camera_ip", c_char_p),      # 下/主相机 IP
        ("camera_ip_up", c_char_p),   # 上相机 IP (装卸一体模式)
        ("depth_file", c_char_p),
        ("color_file", c_char_p)
    ]


# ============================================================
# SDK 类封装
# ============================================================

class Camera3DSDK:
    """Camera3D C-API Python 封装类库"""

    def __init__(self, lib_path: str = None):
        """
        初始化并加载动态库
        :param lib_path: 动态库文件的自定义路径，若不传则自动根据系统类型推导相对路径
        """
        # 获取当前脚本所在目录
        curr_dir = os.path.dirname(os.path.abspath(__file__))

        if sys.platform.startswith("win"):
            # 1. 动态推导并注册依赖项所在的 bin 目录
            # (如果需要写死，也可替换为 bin_dir = r"D:\demo\cubebraid_sdk\bin")
            bin_dir = os.path.abspath(os.path.join(curr_dir, r"D:\demo\dll"))
            
            if hasattr(os, "add_dll_directory") and os.path.exists(bin_dir):
                os.add_dll_directory(bin_dir)

            # 2. 拼接完整的 DLL 路径
            if lib_path is None:
                lib_name = os.path.join(bin_dir, "CameraSDK.dll")
            else:
                lib_name = lib_path
        else:
            if lib_path is None:
                lib_name = os.path.join(curr_dir, "../../bin/libCameraSDK.so")
            else:
                lib_name = lib_path

        # 加载 C++ SDK 动态库
        try:
            self._dll = ctypes.CDLL(os.path.abspath(lib_name))
        except OSError as e:
            print(f"动态库加载失败，请检查路径及依赖: {e}")
            sys.exit(1)

        self._handle = None
        self._bind_functions()

    def _bind_functions(self):
        """绑定 C 导出接口的参数与返回值类型 (与 C-API 头文件严格对应)"""
        # 1. 生命周期接口
        self._dll.Camera3D_Create.restype = c_void_p
        self._dll.Camera3D_Create.argtypes = []

        self._dll.Camera3D_Destroy.restype = None
        self._dll.Camera3D_Destroy.argtypes = [c_void_p]

        # 2. 初始化与连接接口
        self._dll.Camera3D_Initialize.restype = c_int
        self._dll.Camera3D_Initialize.argtypes = [c_void_p, POINTER(Camera3DConfig)]

        self._dll.Camera3D_Connect.restype = c_int
        self._dll.Camera3D_Connect.argtypes = [c_void_p]

        self._dll.Camera3D_Disconnect.restype = None
        self._dll.Camera3D_Disconnect.argtypes = [c_void_p]

        self._dll.Camera3D_IsConnected.restype = c_int
        self._dll.Camera3D_IsConnected.argtypes = [c_void_p]

        # 3. 算法计算接口
        self._dll.Camera3D_ProcessTradition.restype = c_int
        self._dll.Camera3D_ProcessTradition.argtypes = [
            c_void_p,
            POINTER(Camera3DCalibrationPose),
            ctypes.c_char_p,                     # cameraIP (C 字符串)
            c_int,
            c_float,
            c_float,
            c_float,
            c_float,
            c_int,
            POINTER(Camera3DPoint)
        ]

        self._dll.Camera3D_ProcessLastSurface.restype = c_int
        self._dll.Camera3D_ProcessLastSurface.argtypes = [
            c_void_p,
            POINTER(Camera3DCalibrationPose),
            ctypes.c_char_p,                     # cameraIP (C 字符串)
            c_float,
            c_float,
            c_float,
            c_int,
            POINTER(Camera3DPoint)
        ]

        self._dll.Camera3D_ProcessYaw.restype = c_int
        self._dll.Camera3D_ProcessYaw.argtypes = [
            c_void_p,
            POINTER(Camera3DCalibrationPose),
            ctypes.c_char_p,                     # cameraIP (C 字符串)
            c_float,
            c_float,
            c_float,
            c_int,
            POINTER(c_double)
        ]

        # 4. 状态与错误信息接口
        self._dll.Camera3D_GetLastError.restype = c_int
        self._dll.Camera3D_GetLastError.argtypes = [c_void_p, c_char_p, c_int]

        self._dll.Camera3D_GetLastStatus.restype = c_int
        self._dll.Camera3D_GetLastStatus.argtypes = [c_void_p]

        self._dll.Camera3D_StatusToString.restype = c_char_p
        self._dll.Camera3D_StatusToString.argtypes = [c_int]

    def create(self):
        """创建 SDK 实例句柄"""
        if not self._handle:
            self._handle = self._dll.Camera3D_Create()
            if not self._handle:
                raise RuntimeError("创建 Camera3D 实例句柄失败，返回 NULL 指针")

    def initialize(self, camera_ip: str, camera_ip_up: str = "", depth_file: str = "", color_file: str = "") -> int:
        """初始化 SDK 配置"""
        config = Camera3DConfig(
            camera_ip=camera_ip.encode('utf-8'),
            camera_ip_up=camera_ip_up.encode('utf-8'),
            depth_file=depth_file.encode('utf-8'),
            color_file=color_file.encode('utf-8')
        )
        # 使用 byref 传递结构体指针
        return self._dll.Camera3D_Initialize(self._handle, byref(config))

    def connect(self) -> int:
        """连接相机"""
        return self._dll.Camera3D_Connect(self._handle)

    def disconnect(self):
        """断开相机连接"""
        if self._handle:
            self._dll.Camera3D_Disconnect(self._handle)

    def is_connected(self) -> bool:
        """获取相机连接状态 (True: 已连接, False: 未连接)"""
        if not self._handle:
            return False
        return self._dll.Camera3D_IsConnected(self._handle) == 1

    def process_tradition(self, pose: Camera3DCalibrationPose, camera_ip: str, model_mod: int,
                          agv_x: float, agv_y: float, angle: float,
                          j1_angle: float, integrated_mode: bool) -> tuple[int, Camera3DPoint]:
        """
        集装箱内部/斜坡基准点计算
        :param pose: 标定位姿 (Camera3DCalibrationPose)
        :param camera_ip: 相机 IP 地址 (如 "192.168.1.100")
        :param model_mod: 0-第一面顶吸基准点, 1-其它面
        :param agv_x: AGV 导航前向距离
        :param agv_y: AGV 导航左侧距离
        :param angle: 倾角仪角度
        :param j1_angle: AGV 一轴关节角 (单位: 度)
        :param integrated_mode: 是否为装卸一体模式 (True/False)
        :return: (状态码, 计算出的 Point3D 点坐标)
        """
        res_point = Camera3DPoint()
        
        # 字符串必须 encode 为 bytes 才能传给 c_char_p
        ip_bytes = camera_ip.encode('utf-8') if isinstance(camera_ip, str) else camera_ip
    
        status = self._dll.Camera3D_ProcessTradition(
            self._handle,
            byref(pose),
            ip_bytes,
            int(model_mod),
            c_float(agv_x),
            c_float(agv_y),
            c_float(angle),
            c_float(j1_angle),
            int(integrated_mode),
            byref(res_point)
        )
        return status, res_point

    def process_last_surface(self, pose: Camera3DCalibrationPose, camera_ip: str,
                             agv_x: float, agv_y: float,
                             j1_angle: float, integrated_mode: bool) -> tuple[int, Camera3DPoint]:
        """
        最后一面侧吸基准点计算
        :param pose: 标定位姿 (Camera3DCalibrationPose)
        :param camera_ip: 相机 IP 地址 (如 "192.168.1.100")
        :param agv_x: AGV 导航前向距离
        :param agv_y: AGV 导航左侧距离
        :param j1_angle: AGV 一轴关节角 (单位: 度)
        :param integrated_mode: 是否为装卸一体模式 (True/False)
        :return: (状态码, 计算出的 Point3D 点坐标)
        """
        res_point = Camera3DPoint()
        
        # 字符串必须 encode 为 bytes 才能传给 c_char_p
        ip_bytes = camera_ip.encode('utf-8') if isinstance(camera_ip, str) else camera_ip
                
        status = self._dll.Camera3D_ProcessLastSurface(
            self._handle,
            byref(pose),
            ip_bytes,
            c_float(agv_x),
            c_float(agv_y),
            c_float(j1_angle),
            int(integrated_mode),
            byref(res_point)
        )
        return status, res_point

    def process_yaw(self, pose: Camera3DCalibrationPose, camera_ip: str,
                    slam_x: float, slam_y: float,
                    j1_angle: float, integrated_mode: bool) -> tuple[int, float]:
        """
        计算加强筋法向及 AGV 航向角偏差
        :param pose: 标定位姿 (Camera3DCalibrationPose)
        :param camera_ip: 相机 IP 地址 (如 "192.168.1.100")
        :param slam_x: AGV 导航前向距离
        :param slam_y: AGV 导航左侧距离
        :param j1_angle: AGV 一轴关节角 (单位: 度)
        :param integrated_mode: 是否为装卸一体模式 (True/False)
        :return: (状态码, 航向角偏差 Yaw - 单位: 度)
        """
        yaw_out = c_double(0.0)
        
        # 字符串必须 encode 为 bytes 才能传给 c_char_p
        ip_bytes = camera_ip.encode('utf-8') if isinstance(camera_ip, str) else camera_ip
                
        status = self._dll.Camera3D_ProcessYaw(
            self._handle,
            byref(pose),
            ip_bytes,
            c_float(slam_x),
            c_float(slam_y),
            c_float(j1_angle),
            int(integrated_mode),
            byref(yaw_out)
        )
        return status, yaw_out.value

    def get_last_error(self) -> str:
        """获取最后一次详细错误描述字符串"""
        buf = create_string_buffer(512)
        self._dll.Camera3D_GetLastError(self._handle, buf, 512)
        return buf.value.decode('utf-8', errors='ignore')

    def get_last_status(self) -> int:
        """获取最后一次调用的状态码"""
        return self._dll.Camera3D_GetLastStatus(self._handle)

    def status_to_string(self, status: int) -> str:
        """将状态码数值转为描述字符串"""
        res_ptr = self._dll.Camera3D_StatusToString(status)
        if res_ptr:
            return res_ptr.decode('utf-8', errors='ignore')
        return f"Unknown Status Code: {status}"

    def destroy(self):
        """释放 C++ 侧底层资源"""
        if self._handle:
            self._dll.Camera3D_Destroy(self._handle)
            self._handle = None

    def __enter__(self):
        """Python 上下文管理器入口"""
        self.create()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        """Python 上下文管理器出口 (自动断开连接并销毁实例)"""
        self.disconnect()
        self.destroy()