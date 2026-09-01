from jsonparameter import JsonParameterSDK

sdk = JsonParameterSDK(r"../../bin/JsonParameterSDK.dll")

# 手眼标定
pose = sdk.get_calibration(r"./data/json/Hand-eye_calibration_parameters.json", 0)
print("Calibration:")
print(pose)

# SKU
sku = sdk.get_sku(r"./data/json/sku_data.json", 0)
print("SKU:")
print(sku)

# AGV角度
angle = sdk.get_agv_angle(r"./data/json/agv_angle.json", 0)
print("AGV Angle:", angle)

# 获取机器人取料/放料位姿
robot_pose = sdk.get_robot_pose(r"./data/json/setting_parameters.json", 0)
print("机器人取料/放料位姿:", robot_pose)

# 获取倾角仪端口
port = sdk.get_inclinometer_port(r"./data/json/setting_parameters.json", 5)
print("倾角仪端口:", port)

# 获取续码面数/层数
layer_num, surface_num = sdk.get_surface_layer(r"./data/json/rd_demo_data_keba.json", 0)
print("层数:", layer_num)
print("面数:", surface_num)

# 获取机器人状态
robot_state = sdk.get_robot_state(r"./data/json/robot_data.json")
print("机器人当前状态:", robot_state)

# 初始化机器人状态文件
sdk.init_robot_data(r"./data/json/robot_data.json")

# 获取 TXT 续码配置
continuation_config = sdk.get_continuation_config(r"./data/txt/continuation_config.txt")
print("续码参数:", continuation_config)

# 获取特殊面 diff_x
diff_x = sdk.get_stackStyle_diffx(r"./data/json/test_demo.json", 5)
print("左侧突出值:", diff_x)

sdk.close()