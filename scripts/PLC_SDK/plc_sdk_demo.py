import os
import sys
import ctypes
import time
from ctypes import Structure, c_double, c_float, c_int, c_int16, c_bool, c_char_p, c_void_p, POINTER, byref

# =================================================================
# 数据结构映射 (#pragma pack(push, 1) 紧凑对齐)
# =================================================================

class PLCStatus(Structure):
    _pack_ = 1  # 对应 C 语言 #pragma pack(push, 1)
    _fields_ = [
        ("suction_cup_state", c_int16),             # 吸盘状态
        ("fixture_state", c_int16),                 # 夹具/底托状态
        ("robot_pick_state", c_int16),              # 机器人取料状态
        ("robot_leave_state", c_int16),             # 机器人离开状态
        ("table_control_state", c_int16),           # 摆台控制状态
        ("hydraulic_rod_rise_state", c_int16),      # 液压杆上升状态
        ("hydraulic_rod_lower_state", c_int16),     # 液压杆下降状态
        ("plc_init_state", c_int16),                # PLC初始化状态
        ("table_init_control_ok_state", c_int16),   # 摆台初始化完成状态
        ("plc_data_request_state", c_int16),        # PLC数据请求状态
    ]

class PickUpData(Structure):
    _pack_ = 1  # 对应 C 语言 #pragma pack(push, 1)
    _fields_ = [
        ("fetch_mode", c_int),       # 取料模式 (1, 2 为转动；3, 4 为不转动)
        ("mode_switch", c_int),      # 模式切换参数
        ("sku_l", c_double),         # 长 (mm)
        ("sku_w", c_double),         # 宽 (mm)
        ("sku_h", c_double),         # 高 (mm)
        ("sku_num", c_int),          # 数量
        ("sku_weight", c_double),    # 单个重量 (kg)
    ]

# =================================================================
# 加载 PLC SDK 动态库与 C-API 类型绑定
# =================================================================
lib_path = os.path.dirname(os.path.abspath(__file__))

if sys.platform.startswith("win"):
    # PLC SDK 主DLL (保留相对路径格式)
    plc_sdk_path = "../../bin/PLC_SDK.dll"
    # snap7 DLL
    snap7_path = "../../bin/snap7.dll"

    # 将相对路径基于当前文件位置解析，并转为绝对路径
    plc_sdk_abs_path = os.path.abspath(os.path.join(lib_path, plc_sdk_path))
    snap7_abs_path = os.path.abspath(os.path.join(lib_path, snap7_path))

    # 检查文件是否存在
    if not os.path.isfile(plc_sdk_abs_path):
        print(f"\n找不到 PLC_SDK.dll: {plc_sdk_abs_path}\n")
        sys.exit(1)
    if not os.path.isfile(snap7_abs_path):
        print(f"\n找不到 snap7.dll: {snap7_abs_path}\n")
        sys.exit(1)

    # 获取 DLL 所在的文件夹（绝对路径）
    plc_sdk_dir = os.path.dirname(plc_sdk_abs_path)
    snap7_dir = os.path.dirname(snap7_abs_path)

    # 添加 DLL 搜索目录
    dll_dirs = []
    if hasattr(os, "add_dll_directory"):
        # os.add_dll_directory 必须接收绝对路径
        handle1 = os.add_dll_directory(plc_sdk_dir)
        dll_dirs.append(handle1)
        
        if snap7_dir != plc_sdk_dir:
            handle2 = os.add_dll_directory(snap7_dir)
            dll_dirs.append(handle2)
            
        print("\n已添加 DLL 搜索目录:")
        print(f"相对路径: {os.path.dirname(plc_sdk_path)} -> 映射为绝对路径: {plc_sdk_dir}")

    # 先加载 snap7.dll
    try:
        print("\n正在加载 snap7.dll...")
        snap7 = ctypes.CDLL(snap7_abs_path)
        print("snap7.dll 加载成功!")
    except OSError as e:
        print("\nsnap7.dll 加载失败!")
        print(f"错误信息: {e}")
        sys.exit(1)

    # 再加载 PLC_SDK.dll
    try:
        print("\n正在加载 PLC_SDK.dll...")
        sdk = ctypes.CDLL(plc_sdk_abs_path)
        print("PLC_SDK.dll 加载成功!")
    except OSError as e:
        print("\nPLC_SDK.dll 加载失败!")
        print(f"错误信息: {e}")
        sys.exit(1)

# 声明生命周期接口
sdk.plc_create.restype = c_void_p
sdk.plc_create.argtypes = []

sdk.plc_destroy.restype = None
sdk.plc_destroy.argtypes = [c_void_p]

# 连接相关接口
sdk.plc_connect.restype = c_bool
sdk.plc_connect.argtypes = [c_void_p, c_char_p, c_int, c_int]

sdk.plc_disconnect.restype = None
sdk.plc_disconnect.argtypes = [c_void_p]

sdk.plc_is_connected.restype = c_bool
sdk.plc_is_connected.argtypes = [c_void_p]

# 控制离散信号设置接口
sdk.plc_set_open_suction_cup.restype = None
sdk.plc_set_open_suction_cup.argtypes = [c_void_p, c_bool]

sdk.plc_set_close_suction_cup.restype = None
sdk.plc_set_close_suction_cup.argtypes = [c_void_p, c_bool]

sdk.plc_set_fixture_roll_out.restype = None
sdk.plc_set_fixture_roll_out.argtypes = [c_void_p, c_bool]

sdk.plc_set_fixture_initialization.restype = None
sdk.plc_set_fixture_initialization.argtypes = [c_void_p, c_bool]

sdk.plc_set_suction_cup_picking_ok.restype = None
sdk.plc_set_suction_cup_picking_ok.argtypes = [c_void_p, c_bool]

# 业务数据与状态控制接口
sdk.plc_get_status.restype = c_bool
sdk.plc_get_status.argtypes = [c_void_p, POINTER(PLCStatus)]

sdk.plc_control_table_angle.restype = None
sdk.plc_control_table_angle.argtypes = [c_void_p, c_int]

sdk.plc_update_box_state.restype = None
sdk.plc_update_box_state.argtypes = [c_void_p, c_int, c_int, c_int]

sdk.plc_update_inclinometer_angle.restype = None
sdk.plc_update_inclinometer_angle.argtypes = [c_void_p, c_float]

sdk.plc_update_total_box_state.restype = None
sdk.plc_update_total_box_state.argtypes = [c_void_p, c_int, c_int]

sdk.plc_send_pickup_data.restype = c_bool
sdk.plc_send_pickup_data.argtypes = [c_void_p, POINTER(PickUpData)]

# =================================================================
# 3. 业务逻辑控制封装类 (Pythonic Class Wrapper)
# =================================================================

class PLCController:
    def __init__(self):
        """创建 SDK 实例句柄"""
        self._handle = sdk.plc_create()
        if not self._handle:
            raise RuntimeError("创建 PLC 控制器实例句柄失败!")

    def __del__(self):
        """析构时安全释放底层句柄"""
        if hasattr(self, '_handle') and self._handle:
            sdk.plc_destroy(self._handle)
            self._handle = None

    def connect(self, ip: str, rack: int = 0, slot: int = 1) -> bool:
        """连接 PLC"""
        ip_bytes = ip.encode('utf-8')
        return sdk.plc_connect(self._handle, ip_bytes, rack, slot)

    def disconnect(self):
        """断开 PLC 连接"""
        sdk.plc_disconnect(self._handle)

    def is_connected(self) -> bool:
        """查询 PLC 是否连接"""
        return sdk.plc_is_connected(self._handle)

    def set_open_suction_cup(self, enable: bool):
        """开启吸盘离散信号"""
        sdk.plc_set_open_suction_cup(self._handle, enable)
        
    def set_close_suction_cup(self, enable: bool):
        """关闭吸盘离散信号"""
        sdk.plc_set_close_suction_cup(self._handle, enable)
        
    def set_fixture_roll_out(self, enable: bool):
        """底托推出离散信号"""
        sdk.plc_set_fixture_roll_out(self._handle, enable)
        
    def set_fixture_initialization(self, enable: bool):
        """底托缩回离散信号"""
        sdk.plc_set_fixture_initialization(self._handle, enable)

    def control_table_angle(self, angle_mode: int):
        """控制摆台角度模式"""
        sdk.plc_control_table_angle(self._handle, angle_mode)

    def update_box_state(self, surface_num: int, lay_num: int, box_num: int):
        """更新箱数状态"""
        sdk.plc_update_box_state(self._handle, surface_num, lay_num, box_num)

    def update_inclinometer_angle(self, angle_value: float):
        """更新倾角仪角度"""
        sdk.plc_update_inclinometer_angle(self._handle, c_float(angle_value))

    def update_total_box_state(self, totalbox_num: int, remain_num: int):
        """更新总箱数与剩余箱数状态"""
        sdk.plc_update_total_box_state(self._handle, totalbox_num, remain_num)

    def send_pickup_data(self, fetch_mode: int, mode_switch: int, sku_l: float, 
                         sku_w: float, sku_h: float, sku_num: int, sku_weight: float) -> bool:
        """发送取料参数数据"""
        pickup_data = PickUpData(
            fetch_mode=fetch_mode,
            mode_switch=mode_switch,
            sku_l=c_double(sku_l),
            sku_w=c_double(sku_w),
            sku_h=c_double(sku_h),
            sku_num=sku_num,
            sku_weight=c_double(sku_weight)
        )
        return sdk.plc_send_pickup_data(self._handle, byref(pickup_data))

    def get_status(self) -> PLCStatus:
        """获取 PLC 实时状态"""
        status = PLCStatus()
        if sdk.plc_get_status(self._handle, byref(status)):
            return status
        return None

# =================================================================
# 主程序运行 Demo
# =================================================================

if __name__ == "__main__":
    plc_ip = "192.168.30.49"  # 替换为实际 PLC IP

    print("正在初始化 PLC SDK 并在 Python 中调用...")
    plc = PLCController()

    print(f"尝试连接 PLC ({plc_ip})...")
    if plc.connect(plc_ip, rack=0, slot=1):
        print("连接 PLC 成功！")

        # 示例 1: 发送取料参数
        print("发送取料参数数据...")
        success = plc.send_pickup_data(
            fetch_mode=1,
            mode_switch=1,
            sku_l=400.0,
            sku_w=300.0,
            sku_h=200.0,
            sku_num=2,
            sku_weight=5.5
        )
        while not success:
            time.sleep(1)
            print("取料参数发送失败, 正在重新发送...")
            success = plc.send_pickup_data(1, 1, 400.0, 300.0, 200.0, 2, 5.5)
        print(f"取料参数发送结果: {success}")

        # 示例 2: 发送控制与状态信号
        print("触发控制信号与参数更新...")
        plc.set_open_suction_cup(True)
        plc.control_table_angle(3)
        plc.update_box_state(surface_num=1, lay_num=2, box_num=5)
        plc.update_inclinometer_angle(12.5)
        plc.update_total_box_state(totalbox_num=100, remain_num=80)
        
        plc.set_fixture_roll_out(True)
        time.sleep(0.1)

        # 示例 3: 循环监控实时状态
        print("读取 PLC 状态:")
        for i in range(3):
            time.sleep(0.5)
            status = plc.get_status()
            if status:
                print(f"  [{i+1}] 吸盘: {status.suction_cup_state} | "
                      f"夹具: {status.fixture_state} | "
                      f"机器人取料: {status.robot_pick_state} | "
                      f"PLC请求数据: {status.plc_data_request_state}")
                
        plc.set_close_suction_cup(True)
        time.sleep(0.1)
        
        plc.set_fixture_initialization(True)
        time.sleep(0.1)

        # 断开连接
        plc.disconnect()
        print("已断开与 PLC 的连接。")
    else:
        print("连接 PLC 失败，请检查网络设置或 PLC 控制器状态。")