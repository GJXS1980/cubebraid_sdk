# JSONParameter SDK API 接口文档

## 1. 概述

`JSONParameter SDK` 提供了一套用于机器人系统参数配置管理的开发接口，主要用于 JSON 配置文件的读取、解析、修改以及机器人运行参数管理。

本 SDK 采用标准 C API 接口设计，通过动态库形式导出，支持：

- C/C++ 应用程序调用
- Windows DLL 动态链接
- Linux SO 动态链接
- Python (`ctypes`) 调用
- 其他支持 C ABI 的语言进行二次封装


### 核心特性

- **跨平台动态库支持**

  支持 Windows / Linux 平台，通过统一导出宏实现 DLL/SO 动态库调用。


- **标准 C 接口设计**

  所有接口采用 `extern "C"` 导出，避免 C++ 名称修饰（Name Mangling），方便跨语言调用。


- **机器人参数统一管理**

  支持管理：

  - 手眼标定参数
  - SKU 物料尺寸参数
  - AGV 航向角参数
  - 机器人运动位姿
  - 机器人运行状态
  - 续码任务配置
  - 垛型补偿参数


- **完善错误处理机制**

  提供统一错误码和错误信息查询接口，支持：

  - JSON 文件异常检测
  - JSON 格式解析错误
  - 参数缺失检测
  - 索引合法性检查
  - 文件写入异常处理


---

## 2. SDK 使用流程


典型调用流程：

```text
1. JsonParameterSDK_Initialize()

        ↓

2. JsonParameterSDK_CheckJsonFile()

        ↓

3. 获取机器人配置参数

        ↓

4. 修改运行参数（可选）

        ↓

5. JsonParameterSDK_Uninitialize()
```

## 基础数据结构与枚举
### 3.1 JsonParameterSDKError（错误码）
```cpp
enum JsonParameterSDKError
{
    JSONPARAM_SDK_SUCCESS              = 0,

    JSONPARAM_SDK_ERROR_UNKNOWN        = -1,

    JSONPARAM_SDK_ERROR_INVALID_PARAM  = -2,

    JSONPARAM_SDK_ERROR_FILE_NOT_FOUND = -3,

    JSONPARAM_SDK_ERROR_JSON_PARSE     = -4,

    JSONPARAM_SDK_ERROR_JSON_EMPTY     = -5,

    JSONPARAM_SDK_ERROR_FIELD_MISSING  = -6,

    JSONPARAM_SDK_ERROR_INDEX_INVALID  = -7,

    JSONPARAM_SDK_ERROR_BUFFER_SMALL   = -8,

    JSONPARAM_SDK_ERROR_WRITE_FAILED   = -9
};
```
#### 错误码说明：
| 错误码  | 说明         |
| ---- | ---------- |
| `0`  | 执行成功       |
| `-1` | 未知错误       |
| `-2` | 参数错误，例如空指针 |
| `-3` | 文件不存在或无法打开 |
| `-4` | JSON解析失败   |
| `-5` | JSON内容为空   |
| `-6` | 缺少指定字段     |
| `-7` | 索引非法       |
| `-8` | 用户缓存空间不足   |
| `-9` | 文件写入失败     |

## 3.2 CalibrationPose（手眼标定数据）
用于描述相机与机器人之间的标定关系。
```cpp
typedef struct CalibrationPose
{
    float x;
    float y;
    float z;

    float qw;
    float qx;
    float qy;
    float qz;

} CalibrationPose;
```

#### 字段说明：
| 字段 | 类型    | 说明    |
| -- | ----- | ----- |
| x  | float | X方向平移 |
| y  | float | Y方向平移 |
| z  | float | Z方向平移 |
| qw | float | 四元数W  |
| qx | float | 四元数X  |
| qy | float | 四元数Y  |
| qz | float | 四元数Z  |


## 3.3 SkuData（SKU数据）
#### 用于描述物料尺寸和重量。
```cpp
typedef struct SkuData
{
    double length;
    double width;
    double height;
    double weight;

} SkuData;
```
#### 字段说明：
| 字段     | 类型     | 说明 |
| ------ | ------ | -- |
| length | double | 长度 |
| width  | double | 宽度 |
| height | double | 高度 |
| weight | double | 重量 |

## 3.4 RobotPose（机器人位姿）
#### 用于描述机器人目标位置和姿态。

```cpp
typedef struct RobotPose
{
    float x;
    float y;
    float z;

    float rx;
    float ry;
    float rz;

} RobotPose;
```
#### 字段说明：

| 字段 | 说明      |
| -- | ------- |
| x  | 空间 X 坐标 |
| y  | 空间 Y 坐标 |
| z  | 空间 Z 坐标 |
| rx | 绕 X 轴旋转 |
| ry | 绕 Y 轴旋转 |
| rz | 绕 Z 轴旋转 |

## 3.5 ContinuationConfig（续码配置）

```cpp
typedef struct ContinuationConfig
{
    int surface_num;

    int layer_num;

    int action_num;

    int work_mode_num;

    int stack_type;

    int agv_mode_num;

} ContinuationConfig;
```

#### 字段说明：

| 字段            | 说明    |
| ------------- | ----- |
| surface_num   | 面数量   |
| layer_num     | 层数量   |
| action_num    | 动作编号  |
| work_mode_num | 工作模式  |
| stack_type    | 垛型类型  |
| agv_mode_num  | AGV模式 |

## 3.6 RobotState（机器人状态）

```cpp
typedef struct RobotState
{
    int work_mode_state;

    int surface_state;

    int layer_state;

    int action_state;

    int total_num;


    float x;

    float y;

    float z;


    float rx;

    float ry;

    float rz;

} RobotState;
```

#### 字段说明：

| 字段              | 说明     |
| --------------- | ------ |
| work_mode_state | 工作模式状态 |
| surface_state   | 当前面状态  |
| layer_state     | 当前层状态  |
| action_state    | 当前动作状态 |
| total_num       | 累计数量   |
| x/y/z           | 机器人位置  |
| rx/ry/rz        | 机器人姿态  |


## C-API 接口说明

所有接口均采用 C ABI 导出：

```cpp
extern "C"
```

支持：
Windows __declspec(dllexport)
Linux 默认符号导出

## 4.1 SDK 生命周期接口
### JsonParameterSDK_Initialize
```cpp
int JsonParameterSDK_Initialize();
```

#### 功能
初始化 SDK。

#### 说明
该接口需要在所有其他 API 调用之前执行。

#### 返回值
| 返回值                     | 说明    |
| ----------------------- | ----- |
| `JSONPARAM_SDK_SUCCESS` | 初始化成功 |
| 其他错误码                   | 初始化失败 |

### JsonParameterSDK_Uninitialize
```cpp
void JsonParameterSDK_Uninitialize();
```

#### 功能
释放 SDK 资源。

#### 说明
程序退出前建议调用。

## 4.2 错误信息接口
### JsonParameterSDK_GetLastError

```cpp
int JsonParameterSDK_GetLastError(
    char* buffer,
    int buffer_size
);
```

#### 功能
获取最近一次错误信息。

#### 参数
| 参数          | 说明        |
| ----------- | --------- |
| buffer      | 用户提供的输出缓存 |
| buffer_size | 缓存大小      |

#### 返回值
| 返回值 | 说明   |
| --- | ---- |
| 0   | 成功   |
| -2  | 参数错误 |
| -8  | 缓存不足 |


## 4.3 JSON 文件检查接口
### JsonParameterSDK_CheckJsonFile
```cpp
int JsonParameterSDK_CheckJsonFile(
    const char* file_path
);
```

#### 功能
检查 JSON 文件是否存在并验证格式。

#### 参数
| 参数        | 说明       |
| --------- | -------- |
| file_path | JSON文件路径 |

#### 返回值
成功：
```cpp
JSONPARAM_SDK_SUCCESS
```

失败：

返回对应错误码。

## 5. 参数读取接口
### JsonParameterSDK_GetCalibration

```cpp
int JsonParameterSDK_GetCalibration(
    const char* file_path,
    int cam_mode,
    CalibrationPose* result
);
```

#### 功能
获取指定相机模式下的手眼标定数据。

#### 参数
| 参数        | 说明         |
| --------- | ---------- |
| file_path | JSON配置文件路径 |
| cam_mode  | 相机模式编号     |
| result    | 输出标定数据     |


#### 返回值
成功：
```cpp
JSONPARAM_SDK_SUCCESS
```

### JsonParameterSDK_GetSku
```cpp
int JsonParameterSDK_GetSku(
    const char* file_path,
    int sku_index,
    SkuData* result
);
```

#### 功能
获取指定 SKU 参数。

#### 参数
| 参数        | 说明      |
| --------- | ------- |
| file_path | 配置文件    |
| sku_index | SKU索引   |
| result    | 输出SKU数据 |


### JsonParameterSDK_GetAgvAngle
```cpp
int JsonParameterSDK_GetAgvAngle(
    const char* file_path,
    int angle_index,
    float* angle
);
```

#### 功能
获取 AGV 航向角。

#### 参数
| 参数          | 说明   |
| ----------- | ---- |
| file_path   | 配置文件 |
| angle_index | 角度索引 |
| angle       | 输出角度 |

## JsonParameterSDK_GetRobotPose
```cpp
int JsonParameterSDK_GetRobotPose(
    const char* file_path,
    int param_mode,
    RobotPose* result
);
```

#### 功能
获取机器人取料/放料位姿。

#### 参数
| 参数         | 说明      |
| ---------- | ------- |
| file_path  | 配置文件    |
| param_mode | 参数模式    |
| result     | 输出机器人位姿 |


## JsonParameterSDK_GetInclinometerPort
```cpp
int JsonParameterSDK_GetInclinometerPort(
    const char* file_path,
    int port_index,
    char* buffer,
    int buffer_size
);
```

### 功能
获取倾角仪通信端口。

### 参数
| 参数          | 说明   |
| ----------- | ---- |
| file_path   | 配置文件 |
| port_index  | 端口索引 |
| buffer      | 输出端口 |
| buffer_size | 缓存大小 |

## JsonParameterSDK_GetRobotState
```cpp
int JsonParameterSDK_GetRobotState(
    const char* file_path,
    RobotState* result
);
```

### 功能
获取机器人运行状态。

### 输出
```cpp
RobotState
```

## 6. 参数修改接口
### JsonParameterSDK_SetJsonInt
```cpp
int JsonParameterSDK_SetJsonInt(
    const char* file_path,
    const char* field_name,
    int value
);
```

#### 功能
修改 JSON 整数字段。

### JsonParameterSDK_SetJsonFloat
```cpp
int JsonParameterSDK_SetJsonFloat(
    const char* file_path,
    const char* field_name,
    float value
);
```

#### 功能
修改 JSON 浮点字段。

### JsonParameterSDK_SetJsonString
```cpp
int JsonParameterSDK_SetJsonString(
    const char* file_path,
    const char* field_name,
    const char* value
);
```
#### 功能
修改 JSON 字符串字段。

## 7. 续码配置接口
### JsonParameterSDK_GetContinuationConfig

```cpp
int JsonParameterSDK_GetContinuationConfig(
    const char* file_path,
    ContinuationConfig* result
);
```

#### 功能
读取续码配置。

### JsonParameterSDK_GetContinuationSurfaceLayer
```cpp
int JsonParameterSDK_GetContinuationConfig(
    const char* file_path,
    ContinuationConfig* result
);
```

#### 功能
获取续码任务面数和层数。

## 8. 垛型参数接口
### JsonParameterSDK_GetStackStyleDiffX
```cpp
int JsonParameterSDK_GetStackStyleDiffX(
    const char* file_path,
    int index,
    float* diff_x
);
```

#### 功能
获取垛型特殊面的 X 方向补偿量。

#### 应用场景
用于：特殊垛型调整、抓取点补偿和机器人轨迹修正。

## 9. Python ctypes 调用示例
```python
import ctypes
from ctypes import *


sdk = ctypes.CDLL(
    "./JsonParameterSDK.dll"
)


class CalibrationPose(Structure):

    _fields_ = [

        ("x", c_float),
        ("y", c_float),
        ("z", c_float),

        ("qw", c_float),
        ("qx", c_float),
        ("qy", c_float),
        ("qz", c_float)

    ]


sdk.JsonParameterSDK_Initialize()


pose = CalibrationPose()


ret = sdk.JsonParameterSDK_GetCalibration(
    b"camera.json",
    0,
    byref(pose)
)


if ret == 0:

    print(
        pose.x,
        pose.y,
        pose.z
    )


sdk.JsonParameterSDK_Uninitialize()
```