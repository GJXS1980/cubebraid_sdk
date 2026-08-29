import ctypes
import numpy as np
import os
import platform
import sys


# 自动定位编译生成的动态库路径
system_name = platform.system()
is_win = system_name == "Windows"

dll_name = "../../bin/KawasakiKinematicsSDK.dll" if sys.platform == "win32" else "libKawasakiKinematicsSDK.so"
# 此处替换为你生成的 dll / so 文件放置路径
dll_path = os.path.join(os.path.dirname(__file__), dll_name)

lib = ctypes.CDLL(dll_path)

# 函数签名与参数类型定义
lib.create_kinematics_solver.restype = ctypes.c_void_p

lib.destroy_kinematics_solver.argtypes = [ctypes.c_void_p]

lib.fk_calculator.argtypes = [
    ctypes.c_void_p, 
    ctypes.POINTER(ctypes.c_double), 
    ctypes.POINTER(ctypes.c_double)
]

lib.ik_calculator.argtypes = [
    ctypes.c_void_p, 
    ctypes.POINTER(ctypes.c_double), 
    ctypes.POINTER(ctypes.c_double)
]
lib.ik_calculator.restype = ctypes.c_int


# 功能测试
def main():
    # 创建 C++ 求解器对象
    solver = lib.create_kinematics_solver()

    print("=== 川崎 RS080N 机器人运动学 SDK 测试 ===")

    # --- 正运动学计算测试 ---
    q_deg = np.array([-144, -6.48, -96.01, -7.19, -87, 111.97], dtype=np.float64)  # 六个关节角
    out_T16 = np.zeros(16, dtype=np.float64)

    q_ptr = q_deg.ctypes.data_as(ctypes.POINTER(ctypes.c_double))
    t16_ptr = out_T16.ctypes.data_as(ctypes.POINTER(ctypes.c_double))

    lib.fk_calculator(solver, q_ptr, t16_ptr)
    
    T06 = out_T16.reshape((4, 4))
    print("\n正运动学 T06 齐次变换矩阵:\n", np.round(T06, 2))

    # --- 逆运动学计算测试 ---
    # 预分配 8 组关节解的内存 (8x6 = 48 个 double 元素)
    out_solutions = np.zeros(48, dtype=np.float64)
    sol_ptr = out_solutions.ctypes.data_as(ctypes.POINTER(ctypes.c_double))

    num_solutions = lib.ik_calculator(solver, t16_ptr, sol_ptr)
    
    print(f"\n逆运动学成功求解，得到 {num_solutions} 组关节角:")
    solutions_matrix = out_solutions.reshape((8, 6))
    for i in range(num_solutions):
        print(f"解 {i + 1}:", np.round(solutions_matrix[i], 2))

    # 销毁句柄，释放资源
    lib.destroy_kinematics_solver(solver)

if __name__ == "__main__":
    main()