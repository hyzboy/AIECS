# AIECS

一个基于 C++20 和 CMake 的 ECS (Entity Component System) 项目，使用 vcpkg 进行依赖管理，并集成 GLM 数学库。

## 项目特点

- **ECS 架构**: 实现了实体组件系统架构
- **SOA 存储**: TransformStorage 使用 Structure of Arrays 模式，提高缓存性能
- **C++20**: 使用现代 C++ 标准
- **GLM 数学库**: 用于向量、矩阵和四元数运算

## 架构说明

### 核心组件

1. **TransformStorage** (`include/TransformStorage.h`)
   - 使用 SOA (Structure of Arrays) 模式存储 Transform 组件
   - 分离存储位置 (position)、旋转 (rotation) 和缩放 (scale)
   - 支持父子实体关系（parent/children）存储
   - 提供高效的缓存局部性
   - 支持槽位复用机制

2. **Entity** (`include/Entity.h`)
   - 实体类，作为 Transform 数据的句柄
   - 封装 Transform 属性操作（获取、设置、删除）
   - 支持父子实体关系管理（addChild, removeChild, getParent, getChildren）
   - 提供友好的 API，使用起来就像 Entity 自己拥有 Transform 数据

3. **EntityManager** (`include/EntityManager.h`)
   - 管理实体和组件的创建与销毁
   - 内置 TransformStorage
   - 提供创建 Entity 的工厂方法，并自动传递 TransformStorage 指针

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
├── include/                    # 头文件目录
│   ├── TransformStorage.h     # SOA 模式的 Transform 存储
│   ├── Entity.h               # 实体类
│   └── EntityManager.h        # 实体管理器
├── src/
│   └── main.cpp               # 主程序文件（示例用法）
└── README.md                  # 本文件
```

## 使用示例

### 基本用法

```cpp
#include "EntityManager.h"

// 创建 EntityManager
EntityManager entityManager;

// 创建实体
Entity entity1 = entityManager.createEntity();
Entity entity2 = entityManager.createEntity();

// 设置 Transform 属性（就像 Entity 拥有这些数据一样）
entity1.setPosition(glm::vec3(1.0f, 2.0f, 3.0f));
entity1.setScale(glm::vec3(2.0f, 2.0f, 2.0f));

// 获取 Transform 属性
glm::vec3 pos = entity1.getPosition();
glm::vec3 scale = entity1.getScale();

// 设置旋转（使用四元数）
entity2.setRotation(glm::angleAxis(glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

// 删除 Transform（标记为可复用）
entity2.deleteTransform();

// 销毁实体
entityManager.destroyEntity(entity1);
```

### 父子实体关系

```cpp
// 创建父实体和子实体
Entity parent = entityManager.createEntity();
Entity child1 = entityManager.createEntity();
Entity child2 = entityManager.createEntity();

// 添加子实体
parent.addChild(child1);
parent.addChild(child2);

// 检查父子关系
bool hasParent = child1.hasParent();  // true
size_t childCount = parent.getChildCount();  // 2

// 获取父实体
Entity parentEntity = child1.getParent();

// 获取所有子实体
std::vector<Entity> children = parent.getChildren();

// 移除子实体（只移除关系，不删除实体）
parent.removeChild(child1);

// 设置新的父实体
child1.setParent(parent);
```

## 依赖库

- **GLM** (OpenGL Mathematics): 用于图形和游戏开发的数学库

## 许可证

根据您的需要添加适当的许可证。
