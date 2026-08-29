# 编译说明
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

# SDK API说明

## AGV SDK API说明
见[AGV Controller SDK API 接口文档](https://github.com/GJXS1980/cubebraid_sdk/blob/main/doc/AGV_SDK_API_Documentation.md)

## JsonParameter SDK API说明
见[JSONParameter SDK API 接口文档](https://github.com/GJXS1980/cubebraid_sdk/blob/main/doc/JsonParameter_SDK_API_Documentation.md)

## 其它API更新中...