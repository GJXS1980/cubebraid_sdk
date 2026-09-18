# Windows编译说明
## 1. 创建并进入构建目录
```bash
mkdir build
cd build
```

## 2. 生成 Visual Studio 2022 的 64 位 .sln
```bash
cmake -G "Visual Studio 17 2022" -A x64 ..

# 如果使用的是 VS 2019，请运行这一行：
# cmake -G "Visual Studio 16 2019" -A x64 ..
```

## 3. 进入build文件夹打开 cubebraid_sdk.sln 进行demo程序编译

# Ubuntu 编译说明
## 安装依赖
```bash
sudo apt update
sudo apt install -y build-essential cmake
```
## 创建并进入构建目录
```bash
mkdir build
cd build
```
## 编译项目
```bash
# 用 CMake 生成 Makefile
cmake -DCMAKE_BUILD_TYPE=Release ..

# 编译 Demo 程序
# 使用 make 进行编译（-j4 表示使用 4 个 CPU 核心并行编译，可根据实际配置调整）
make -j4

# 运行 Demo 程序
# 进入 Demo 输出目录
cd SDK_Demos

# 运行对应的 Demo（以 logger_demo 为例）
./logger_demo
```


# SDK API说明

## 1. AGV SDK API说明
API接口说明见： [AGV Controller SDK API 接口文档](https://github.com/GJXS1980/cubebraid_sdk/blob/main/doc/AGV_SDK_API_Documentation.md)

## 2. JsonParameter SDK API说明
API接口说明见：[JSONParameter SDK API 接口文档](https://github.com/GJXS1980/cubebraid_sdk/blob/main/doc/JsonParameter_SDK_API_Documentation.md)

## 其它API更新中...