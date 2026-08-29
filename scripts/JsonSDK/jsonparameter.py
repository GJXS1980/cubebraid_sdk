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

        # 初始化
        self.dll.JsonParameterSDK_Initialize.restype = ctypes.c_int


        # GetCalibration
        self.dll.JsonParameterSDK_GetCalibration.argtypes = [
            ctypes.c_char_p,
            ctypes.c_int,
            ctypes.POINTER(CalibrationPose)
        ]

        self.dll.JsonParameterSDK_GetCalibration.restype = ctypes.c_int


        # GetSku
        self.dll.JsonParameterSDK_GetSku.argtypes = [
            ctypes.c_char_p,
            ctypes.c_int,
            ctypes.POINTER(SkuData)
        ]

        self.dll.JsonParameterSDK_GetSku.restype = ctypes.c_int


        # GetAgvAngle
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


    def close(self):

        self.dll.JsonParameterSDK_Uninitialize()