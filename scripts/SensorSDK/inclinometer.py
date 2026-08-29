# -*- coding: utf-8 -*-

import ctypes
import os

class Inclinometer:
    """
    DXL360 倾角仪 Python SDK

    底层：
        InclinometerSDK.dll

    调用：
        ctypes
    """
    def __init__(self, dll_path):
        dll_path = os.path.abspath(dll_path)
        if not os.path.exists(dll_path):
            raise FileNotFoundError("找不到 DLL: {}".format(dll_path))

        # Windows DLL
        self.dll = ctypes.CDLL(dll_path)
        self._setup_functions()
        self.handle = (self.dll.Inclinometer_Create())

        if not self.handle:
            raise RuntimeError("创建 Inclinometer SDK 对象失败")

    # ==========================================================
    # 设置DLL函数参数
    # ==========================================================
    def _setup_functions(self):
        # ------------------------------------------------------
        # Create
        # ------------------------------------------------------
        self.dll.Inclinometer_Create.argtypes = []
        self.dll.Inclinometer_Create.restype = (ctypes.c_void_p)

        # ------------------------------------------------------
        # Destroy
        # ------------------------------------------------------
        self.dll.Inclinometer_Destroy.argtypes = [ctypes.c_void_p]
        self.dll.Inclinometer_Destroy.restype = None

        # ------------------------------------------------------
        # Start
        # -----------------------------------------------------
        self.dll.Inclinometer_Start.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_int, ctypes.c_int]
        self.dll.Inclinometer_Start.restype = (ctypes.c_int)

        # ------------------------------------------------------
        # Stop
        # ------------------------------------------------------
        self.dll.Inclinometer_Stop.argtypes = [ctypes.c_void_p]
        self.dll.Inclinometer_Stop.restype = None

        # ------------------------------------------------------
        # IsRunning
        # ------------------------------------------------------
        self.dll.Inclinometer_IsRunning.argtypes = [ctypes.c_void_p]
        self.dll.Inclinometer_IsRunning.restype = (ctypes.c_int)

        # ------------------------------------------------------
        # GetX
        # ------------------------------------------------------
        self.dll.Inclinometer_GetXAngle.argtypes = [ctypes.c_void_p]
        self.dll.Inclinometer_GetXAngle.restype = (ctypes.c_float)

        # ------------------------------------------------------
        # GetY
        # ------------------------------------------------------
        self.dll.Inclinometer_GetYAngle.argtypes = [ctypes.c_void_p]
        self.dll.Inclinometer_GetYAngle.restype = (ctypes.c_float)

        # ------------------------------------------------------
        # GetAngle
        # ------------------------------------------------------
        self.dll.Inclinometer_GetAngle.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_float), ctypes.POINTER(ctypes.c_float)]
        self.dll.Inclinometer_GetAngle.restype = None

        # ------------------------------------------------------
        # Reset
        # ------------------------------------------------------
        self.dll.Inclinometer_ResetAngle.argtypes = [ctypes.c_void_p]
        self.dll.Inclinometer_ResetAngle.restype = None

        # ------------------------------------------------------
        # Version
        # ------------------------------------------------------
        self.dll.Inclinometer_GetVersion.argtypes = []
        self.dll.Inclinometer_GetVersion.restype = (ctypes.c_char_p)

    # ==========================================================
    # 获取版本
    # ==========================================================
    def get_version(self):
        version = (self.dll.Inclinometer_GetVersion())
        if version is None:
            return ""
        return version.decode("ascii")

    # ==========================================================
    # 启动
    # ==========================================================
    def start(self, port, baudrate=9600, auto_reconnect=True):
        if self.handle is None:
            raise RuntimeError("SDK对象已经释放")
        # ------------------------------------------------------
        # Windows串口
        # ------------------------------------------------------
        if isinstance(port, str):
            port_bytes = port.encode("ascii")
        else:
            port_bytes = port

        result = (self.dll.Inclinometer_Start(self.handle, port_bytes, int(baudrate), 1 if auto_reconnect else 0))
        if result == 0:
            raise RuntimeError("启动倾角仪失败，串口：{}".format(port))
    # ==========================================================
    # 停止
    # ==========================================================
    def stop(self):
        if self.handle is None:
            return
        self.dll.Inclinometer_Stop(self.handle)

    # ==========================================================
    # 判断运行状态
    # ==========================================================
    def is_running(self):
        if self.handle is None:
            return False
        return bool(self.dll.Inclinometer_IsRunning(self.handle))

    # ==========================================================
    # 获取X角度
    # ==========================================================
    def get_x_angle(self):
        if self.handle is None:
            raise RuntimeError("SDK对象已经释放")
        return float(self.dll.Inclinometer_GetXAngle(self.handle))
    
    # ==========================================================
    # 获取Y角度
    # ==========================================================
    def get_y_angle(self):
        if self.handle is None:
            raise RuntimeError("SDK对象已经释放")
        return float(self.dll.Inclinometer_GetYAngle(self.handle))

    # ==========================================================
    # 同时获取XY
    # ==========================================================
    def get_angle(self):
        if self.handle is None:
            raise RuntimeError("SDK对象已经释放")

        x = ctypes.c_float()
        y = ctypes.c_float()

        self.dll.Inclinometer_GetAngle(self.handle, ctypes.byref(x), ctypes.byref(y))
        
        return (float(x.value), float(y.value))

    # ==========================================================
    # 重置角度
    # ==========================================================
    def reset_angle(self):
        if self.handle is None:
            return

        self.dll.Inclinometer_ResetAngle(self.handle)

    # ==========================================================
    # 关闭SDK
    # ==========================================================
    def close(self):

        if self.handle is None:
            return

        try:
            self.dll.Inclinometer_Stop(self.handle)
        except Exception:
            pass

        try:
            self.dll.Inclinometer_Destroy(self.handle)
        except Exception:
            pass

        self.handle = None


    # ==========================================================
    # 支持 with
    # ==========================================================
    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    # ==========================================================
    # 析构
    # ==========================================================
    def __del__(self):
        try:
            self.close()
        except Exception:
            pass