# AIECS

一个基于 C++20 和 CMake 的项目，使用 vcpkg 进行依赖管理，并集成 GLM 数学库。

## 项目要求

- CMake 3.21 或更高版本
- C++20 兼容的编译器（如 MSVC 2019/2022, GCC 10+, Clang 12+）
- vcpkg 包管理器

## 在 Windows 上使用 vcpkg 构建

### 1. 安装 vcpkg

如果还没有安装 vcpkg，请先安装：

```powershell
# 克隆 vcpkg 仓库
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg

# 运行 bootstrap 脚本
.\bootstrap-vcpkg.bat

# 将 vcpkg 添加到 PATH（可选）
# 或者记住 vcpkg.exe 的完整路径
```

### 2. 配置项目

使用 vcpkg 的 CMake 工具链文件来配置项目：

```powershell
# 创建构建目录
mkdir build
cd build

# 配置 CMake（使用 vcpkg 工具链）
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg安装路径]/scripts/buildsystems/vcpkg.cmake
```

例如，如果 vcpkg 安装在 `C:\vcpkg`：

```powershell
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
```

### 3. 构建项目

```powershell
# 构建
cmake --build .

# 或者使用 Visual Studio
cmake --build . --config Release
```

### 4. 运行程序

```powershell
# Debug 模式
.\bin\Debug\aiecs.exe

# Release 模式
.\bin\Release\aiecs.exe
```

## 使用 Visual Studio

也可以使用 Visual Studio 2019/2022 打开项目：

1. 打开 Visual Studio
2. 选择 "打开本地文件夹"
3. 选择项目根目录
4. Visual Studio 会自动检测 `vcpkg.json` 并安装依赖
5. 确保在 CMake 设置中配置了 vcpkg 工具链文件路径

## 项目结构

```
AIECS/
├── CMakeLists.txt              # CMake 配置文件
├── vcpkg.json                  # vcpkg 依赖清单
├── vcpkg-configuration.json    # vcpkg 配置
├── src/
│   └── main.cpp               # 主程序文件
└── README.md                  # 本文件
```

## 依赖库

- **GLM** (OpenGL Mathematics): 用于图形和游戏开发的数学库

## 许可证

根据您的需要添加适当的许可证。
