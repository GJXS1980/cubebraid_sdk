import ctypes
import os


class CalibrationPose(ctypes.Structure):
    _fields_ = [
        ("x", ctypes.c_float),
        ("y", ctypes.c_float),
        ("z", ctypes.c_float),

        ("qw", ctypes.c_float),
        ("qx", ctypes.c_float),
        ("qy", ctypes.c_float),
        ("qz", ctypes.c_float),
    ]


class SkuData(ctypes.Structure):
    _fields_ = [
        ("length", ctypes.c_double),
        ("width", ctypes.c_double),
        ("height", ctypes.c_double),
        ("weight", ctypes.c_double),
    ]


class RobotPose(ctypes.Structure):
    _fields_ = [
        ("x", ctypes.c_float),
        ("y", ctypes.c_float),
        ("z", ctypes.c_float),

        ("rx", ctypes.c_float),
        ("ry", ctypes.c_float),
        ("rz", ctypes.c_float),
    ]


class RobotState(ctypes.Structure):
    _fields_ = [
        ("work_mode_state", ctypes.c_int),
        ("surface_state", ctypes.c_int),
        ("layer_state", ctypes.c_int),
        ("action_state", ctypes.c_int),
        ("total_num", ctypes.c_int),

        ("x", ctypes.c_float),
        ("y", ctypes.c_float),
        ("z", ctypes.c_float),

        ("rx", ctypes.c_float),
        ("ry", ctypes.c_float),
        ("rz", ctypes.c_float),
    ]
    

class ContinuationConfig(ctypes.Structure):
    _fields_ = [
        ("surface_num", ctypes.c_int),
        ("layer_num", ctypes.c_int),
        ("work_mode_num", ctypes.c_int),
        ("agv_mode_num", ctypes.c_int),
        ("action_num", ctypes.c_int),
        ("stack_type", ctypes.c_int),
    ]


class JsonParameterSDK:

    def __init__(self, dll_path=None):

        if dll_path is None:
            current_dir = os.path.dirname(
                os.path.abspath(__file__)
            )

            dll_path = os.path.join(
                current_dir,
                "JsonParameterSDK.dll"
            )

        self.dll = ctypes.WinDLL(dll_path)

        self._init_functions()

        ret = self.dll.JsonParameterSDK_Initialize()

        if ret != 0:
            raise RuntimeError("SDK initialization failed")


    def _init_functions(self):

        # 初始化SDK
        self.dll.JsonParameterSDK_Initialize.restype = ctypes.c_int

        # 获取手眼标定参数
        self.dll.JsonParameterSDK_GetCalibration.argtypes = [
            ctypes.c_char_p,
            ctypes.c_int,
            ctypes.POINTER(CalibrationPose)
        ]
        self.dll.JsonParameterSDK_GetCalibration.restype = ctypes.c_int


        # 获取 SKU 产品数据
        self.dll.JsonParameterSDK_GetSku.argtypes = [
            ctypes.c_char_p,
            ctypes.c_int,
            ctypes.POINTER(SkuData)
        ]
        self.dll.JsonParameterSDK_GetSku.restype = ctypes.c_int


        # 获取 AGV 航向角
        self.dll.JsonParameterSDK_GetAgvAngle.argtypes = [
            ctypes.c_char_p,
            ctypes.c_int,
            ctypes.POINTER(ctypes.c_float)
        ]
        self.dll.JsonParameterSDK_GetAgvAngle.restype = ctypes.c_int


        # 获取错误
        self.dll.JsonParameterSDK_GetLastError.argtypes = [
            ctypes.c_char_p,
            ctypes.c_int
        ]
        self.dll.JsonParameterSDK_GetLastError.restype = ctypes.c_int
        
        # 获取机器人取料/放料位姿
        self.dll.JsonParameterSDK_GetRobotPose.argtypes = [
            ctypes.c_char_p,
            ctypes.c_int,
            ctypes.POINTER(RobotPose)
        ]
        self.dll.JsonParameterSDK_GetRobotPose.restype = ctypes.c_int
        
        # 获取倾角仪端口
        self.dll.JsonParameterSDK_GetInclinometerPort.argtypes = [
            ctypes.c_char_p,
            ctypes.c_int,
            ctypes.c_char_p
        ]
        self.dll.JsonParameterSDK_GetInclinometerPort.restype = ctypes.c_int
        
        # 获取续码面数/层数
        self.dll.JsonParameterSDK_GetContinuationSurfaceLayer.argtypes = [
            ctypes.c_char_p,
            ctypes.c_int,
            ctypes.POINTER(ctypes.c_int),
            ctypes.POINTER(ctypes.c_int)
        ]
        self.dll.JsonParameterSDK_GetContinuationSurfaceLayer.restype = ctypes.c_int
        
        # 获取机器人状态
        self.dll.JsonParameterSDK_GetRobotState.argtypes = [
            ctypes.c_char_p,
            ctypes.POINTER(RobotState)
        ]
        self.dll.JsonParameterSDK_GetRobotState.restype = ctypes.c_int
        
        # 初始化机器人状态文件
        self.dll.JsonParameterSDK_InitRobotData.argtypes = [
            ctypes.c_char_p
        ]
        self.dll.JsonParameterSDK_InitRobotData.restype = ctypes.c_int
        
        # 获取 TXT 续码配置
        self.dll.JsonParameterSDK_GetContinuationConfig.argtypes = [
            ctypes.c_char_p,
            ctypes.POINTER(ContinuationConfig)
        ]
        self.dll.JsonParameterSDK_GetContinuationConfig.restype = ctypes.c_int
        
        # 获取垛型特殊面 diff_x
        self.dll.JsonParameterSDK_GetStackStyleDiffX.argtypes = [
            ctypes.c_char_p,
            ctypes.c_int,
            ctypes.POINTER(ctypes.c_float)
        ]
        self.dll.JsonParameterSDK_GetStackStyleDiffX.restype = ctypes.c_int


    def get_last_error(self):
        buffer = ctypes.create_string_buffer(2048)
        self.dll.JsonParameterSDK_GetLastError(
            buffer,
            2048
        )

        return buffer.value.decode(
            "utf-8",
            errors="ignore"
        )


    def get_calibration(self, file_path, cam_mode):

        result = CalibrationPose()

        ret = self.dll.JsonParameterSDK_GetCalibration(
            file_path.encode("utf-8"),
            cam_mode,
            ctypes.byref(result)
        )

        if ret != 0:
            raise RuntimeError(self.get_last_error())

        return {
            "x": result.x,
            "y": result.y,
            "z": result.z,

            "qw": result.qw,
            "qx": result.qx,
            "qy": result.qy,
            "qz": result.qz,
        }


    def get_sku(self, file_path, sku_index=0):

        result = SkuData()

        ret = self.dll.JsonParameterSDK_GetSku(
            file_path.encode("utf-8"),
            sku_index,
            ctypes.byref(result)
        )

        if ret != 0:
            raise RuntimeError(self.get_last_error())

        return {
            "length": result.length,
            "width": result.width,
            "height": result.height,
            "weight": result.weight
        }


    def get_agv_angle(self, file_path, index=0):
        angle = ctypes.c_float()

        ret = self.dll.JsonParameterSDK_GetAgvAngle(
            file_path.encode("utf-8"),
            index,
            ctypes.byref(angle)
        )

        if ret != 0:
            raise RuntimeError(self.get_last_error())

        return angle.value
    
    # 获取机器人取料/放料位姿
    def get_robot_pose(self, file_path, param_mode=0):
        result = RobotPose()
        ret = self.dll.JsonParameterSDK_GetRobotPose(
            file_path.encode("utf-8"),
            param_mode,
            ctypes.byref(result)
        )

        if ret != 0:
            raise RuntimeError(self.get_last_error())

        return {
            "x": result.x,
            "y": result.y,
            "z": result.z,
            "rx": result.rx,
            "ry": result.ry,
            "rz": result.rz
        }
    
    # 获取倾角仪端口
    def get_inclinometer_port(self, file_path, index=5):
        # port = ctypes.c_char_p()
        port = ctypes.create_string_buffer(256)

        ret = self.dll.JsonParameterSDK_GetInclinometerPort(
            file_path.encode("utf-8"),
            index,
            port
            # ctypes.byref(port)
        )

        if ret != 0:
            raise RuntimeError(self.get_last_error())
        # return port
        # return port.decode("utf-8")
        return port.value.decode("utf-8")
    
    # 获取续码面数/层数
    def get_surface_layer(self, file_path, index=0):
        layer_num = ctypes.c_int()
        surface_num = ctypes.c_int()

        ret = self.dll.JsonParameterSDK_GetContinuationSurfaceLayer(
            file_path.encode("utf-8"),
            index,
            ctypes.byref(layer_num),
            ctypes.byref(surface_num)
        )

        if ret != 0:
            raise RuntimeError(self.get_last_error())
        return layer_num.value, surface_num.value

    # 获取机器人状态
    def get_robot_state(self, file_path):
        result = RobotState()

        ret = self.dll.JsonParameterSDK_GetRobotState(
            file_path.encode("utf-8"),
            ctypes.byref(result)
        )

        if ret != 0:
            raise RuntimeError(self.get_last_error())
        
        return {
            "work_mode": result.work_mode_state,
            "surface": result.surface_state,
            "layer": result.layer_state,
            "action": result.action_state,
            "total_num": result.total_num,
            "x": result.x,
            "y": result.y,
            "z": result.z,
            "rx": result.rx,
            "ry": result.ry,
            "rz": result.rz
        }
            
    # 初始化机器人状态文件
    def init_robot_data(self, file_path):    
        ret = self.dll.JsonParameterSDK_InitRobotData(
            file_path.encode("utf-8")
        )

        if ret != 0:
            raise RuntimeError(self.get_last_error())
        
        return ret

    # 获取 TXT 续码配置
    def get_continuation_config(self, file_path):
        result = ContinuationConfig()

        ret = self.dll.JsonParameterSDK_GetContinuationConfig(
            file_path.encode("utf-8"),
            ctypes.byref(result)
        )

        if ret != 0:
            raise RuntimeError(self.get_last_error())
        
        return {
            "surface_num": result.surface_num,
            "layer_num": result.layer_num,
            "work_mode_num": result.work_mode_num,
            "agv_mode_num": result.agv_mode_num,
            "action_num": result.action_num,
            "stack_type": result.stack_type
        }
        
    # 获取特殊面 diff_x
    def get_stackStyle_diffx(self, file_path, index=0):
        diff_x = ctypes.c_float()

        ret = self.dll.JsonParameterSDK_GetStackStyleDiffX(
            file_path.encode("utf-8"),
            index,
            ctypes.byref(diff_x)
        )

        if ret != 0:
            raise RuntimeError(self.get_last_error())
        return diff_x.value


    def close(self):
        # 释放 SDK
        self.dll.JsonParameterSDK_Uninitialize()