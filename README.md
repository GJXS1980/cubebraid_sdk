# 编译说明
## 1. 创建并进入构建目录
mkdir build
cd build

## 2. 生成 Visual Studio 2022 的 64 位 .sln
cmake -G "Visual Studio 17 2022" -A x64 ..

# 如果使用的是 VS 2019，请运行这一行：
# cmake -G "Visual Studio 16 2019" -A x64 ..

## 3. 进入build文件夹打开 cubebraid_sdk.sln 进行demo程序编译

# SDK API说明

