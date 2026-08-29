import os
import sys
import ctypes

class LoggerSDK:
    """ Python 端的 C++ Logger SDK 包装类 """
    def __init__(self, lib_path: str):
        if not os.path.exists(lib_path):
            raise FileNotFoundError(f"找不到动态库文件: {lib_path}")

        # 加载 C++ 导出的动态链接库
        self._lib = ctypes.CDLL(lib_path)

        # 设置 C API logger_init 函数签名
        self._lib.logger_init.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
        self._lib.logger_init.restype = None

        # 设置 C API logger_log 函数签名
        self._lib.logger_log.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
        self._lib.logger_log.restype = None

        # 设置 C API logger_emergency_stop 函数签名
        self._lib.logger_emergency_stop.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
        self._lib.logger_emergency_stop.restype = None

    def init(self, log_path: str, instance_name: str = "systemlogger"):
        """ 初始化日志输出路径 """
        name_bytes = instance_name.encode('utf-8')
        path_bytes = log_path.encode('utf-8')
        self._lib.logger_init(name_bytes, path_bytes)

    def log(self, msg: str, instance_name: str = "systemlogger"):
        """ 写入普通日志 """
        name_bytes = instance_name.encode('utf-8')
        msg_bytes = msg.encode('utf-8')
        self._lib.logger_log(name_bytes, msg_bytes)

    def emergency_stop(self, reason: str, instance_name: str = "systemlogger"):
        """ 记录紧急停止信息 """
        name_bytes = instance_name.encode('utf-8')
        reason_bytes = reason.encode('utf-8')
        self._lib.logger_emergency_stop(name_bytes, reason_bytes)


# 测试调用主程序
if __name__ == "__main__":
    # 根据操作系统匹配动态库后缀
    dll_name = "../../bin/LoggerSDK.dll" if sys.platform == "win32" else "libLoggerSDK.so"
    # 此处替换为你生成的 dll / so 文件放置路径
    dll_path = os.path.join(os.path.dirname(__file__), dll_name)

    try:
        # 实例化并初始化 SDK
        logger = LoggerSDK(dll_path)
        logger.init("./log/logger_demo.log")

        # 写入普通日志
        logger.log("Python SDK 连接成功，控制节点已启动！")
        
        # 格式化 Python 数据并写入日志
        sensor_data = [24.5, 25.1, 23.8]
        logger.log(f"当前传感器采集温度: {sensor_data}")

        # 触发紧急停止日志
        logger.emergency_stop("检测到电机过载，发送停止指令！")

    except Exception as e:
        print(f"调用 SDK 过程发生错误: {e}")