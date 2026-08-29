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

sdk.close()