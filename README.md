# AIECS - Frostbite Architecture Edition

一个基于 C++20 和 CMake 的游戏引擎项目，迁移到 Frostbite 架构模式，使用 vcpkg 进行依赖管理，并集成 GLM 数学库。

**包含两个版本：**
- `aiecs_original` - 原始 ECS 架构（基于 SOA）
- `aiecs_frostbite` - Frostbite 架构版本（推荐）

## 项目特点

### Frostbite 版本（推荐）
- **对象系统**: 统一的 Object 基类，提供生命周期管理
- **模块化设计**: Module 系统支持独立的引擎子系统
- **事件驱动**: 完整的 EventSystem 支持发布-订阅模式
- **世界管理**: World 类管理所有对象和模块
- **组件系统**: 动态组件管理，支持任意组件组合
- **层级系统**: 完整的父子关系和变换继承
- **C++20**: 使用现代 C++ 标准
- **GLM 数学库**: 用于向量、矩阵和四元数运算
- **坐标系统**: 右手坐标系，Z 轴向上

### 原始版本
- **纯组件 ECS 架构**: 真正的实体组件系统
- **SOA 存储**: 所有组件使用 Structure of Arrays 模式
- **高性能**: 缓存友好的数据布局

## 架构说明

### Frostbite 架构核心

1. **Object** (`include/Object.h`)
   - 所有游戏对象的基类
   - 提供唯一ID和名称管理
   - 定义生命周期回调：onCreate/onUpdate/onDestroy

2. **Module** (`include/Module.h`)
   - 引擎系统的基类
   - 支持初始化、更新和关闭
   - 用于物理、音频、渲染等子系统

3. **EventSystem** (`include/EventSystem.h`)
   - 事件分发系统
   - 支持事件订阅和发送
   - 包含事件队列用于异步处理

4. **World** (`include/World.h`)
   - 游戏世界管理器
   - 管理所有游戏对象
   - 管理和更新所有模块
   - 中心访问点

5. **GameEntity** (`include/GameEntity.h`)
   - 游戏对象实体
   - 支持动态组件管理
   - 模板化组件访问

6. **组件系统**
   - **TransformComponentFB** - 3D 变换管理
     - 本地/世界坐标变换
     - 父子关系
     - 自动矩阵更新
   - **CollisionComponentFB** - 碰撞数据
   - **RenderComponentFB** - 渲染数据

### 原始 ECS 架构（参考）

1. **EntityContext** - 存储系统上下文
2. **Entity** - 轻量级实体句柄
3. **组件存储 (SOA)** - TransformStorage、CollisionStorage、RenderStorage
4. **组件访问器** - TransformComponent、CollisionComponent、RenderComponent
5. **EntityManager** - 实体和组件生命周期管理
   - 提供批量更新所有根实体变换的方法
   - 管理实体和组件的创建与销毁
   - 内置 TransformStorage 和 EntityContext
   - 提供创建 Entity 的工厂方法，并自动传递 EntityContext 指针
   - 提供批量更新所有根实体变换的方法（updateAllTransforms）
   - 确保所有 Entity 都通过正确的方式创建

## 图形 API 与坐标系统规范

本项目针对 **Vulkan** 图形 API 进行设计，遵循以下规范：

- **图形 API**: Vulkan
- **坐标系统**: 右手坐标系（Right-handed coordinate system）
- **轴向定义**: Z 轴向上（Z-up）
- **深度范围**: 0 到 1（Vulkan 标准深度范围）

### 坐标系统说明

右手坐标系，Z 轴向上的配置意味着：
- **X 轴**: 水平方向（通常向右为正）
- **Y 轴**: 前后方向（通常向前为正）
- **Z 轴**: 垂直方向（向上为正）

这种配置常用于建模和模拟应用，其中垂直高度自然映射到 Z 轴。

### Vulkan 深度范围

Vulkan 使用 0 到 1 的深度范围：
- **0.0**: 最近的深度（near plane）
- **1.0**: 最远的深度（far plane）

这与 OpenGL 的 -1 到 1 范围不同，在设置投影矩阵时需要注意。

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
│   ├── ComponentTypes.h        # 组件类型定义
│   ├── EntityContext.h         # 实体上下文结构
│   ├── Entity.h                # 实体类（仅存储组件索引）
│   ├── EntityManager.h         # 实体管理器
│   ├── TransformStorage.h      # Transform 组件存储（SOA）
│   ├── TransformComponent.h    # Transform 组件访问器
│   ├── CollisionStorage.h      # Collision 组件存储（SOA）
│   ├── CollisionComponent.h    # Collision 组件访问器
│   ├── RenderStorage.h         # Render 组件存储（SOA）
│   └── RenderComponent.h       # Render 组件访问器
├── src/
│   └── main.cpp                # 主程序文件（示例用法）
└── README.md                   # 本文件
```

## 使用示例

### 基本用法 - 创建实体和添加组件

```cpp
#include "EntityManager.h"

// 创建 EntityManager
EntityManager entityManager;

// 创建实体（不带任何组件）
Entity entity = entityManager.createEntity();

// 添加组件
entityManager.addTransformComponent(entity);
entityManager.addRenderComponent(entity);
entityManager.addCollisionComponent(entity);

// 检查组件是否存在
bool hasTransform = entity.hasTransformComponent();  // true
bool hasRender = entity.hasRenderComponent();        // true
```

### 访问和操作 TransformComponent

```cpp
// 获取 TransformComponent
auto transform = entity.getTransformComponent();
if (transform) {
    // 设置本地变换（相对父级）
    transform->setLocalPosition(glm::vec3(10.0f, 5.0f, 2.0f));
    transform->setLocalRotation(glm::angleAxis(glm::radians(45.0f), glm::vec3(0, 0, 1)));
    transform->setLocalScale(glm::vec3(2.0f, 2.0f, 2.0f));
    
    // 获取本地变换
    glm::vec3 pos = transform->getLocalPosition();
    glm::quat rot = transform->getLocalRotation();
    glm::vec3 scale = transform->getLocalScale();
    
    // 设置世界变换（自动反推本地变换）
    transform->setWorldPosition(glm::vec3(100.0f, 50.0f, 0.0f));
    
    // 获取世界变换
    glm::vec3 worldPos = transform->getWorldPosition();
    glm::mat4 worldMatrix = transform->getWorldMatrix();
}
```

### 访问和操作 RenderComponent

```cpp
// 获取 RenderComponent
auto render = entity.getRenderComponent();
if (render) {
    render->setMeshName("cube.mesh");
    render->setMaterialName("metal.mat");
    render->setVisible(true);
    render->setCastShadows(true);
    
    std::string meshName = render->getMeshName();
    bool visible = render->isVisible();
}
```

### 访问和操作 CollisionComponent

```cpp
// 获取 CollisionComponent
auto collision = entity.getCollisionComponent();
if (collision) {
    collision->setBoundingBox(glm::vec3(-1, -1, -1), glm::vec3(1, 1, 1));
    collision->setCollisionLayer(1);
    collision->setEnabled(true);
    
    glm::vec3 bbMin = collision->getBoundingBoxMin();
    uint32_t layer = collision->getCollisionLayer();
    bool enabled = collision->isEnabled();
}
```

### 移除组件

```cpp
// 移除组件
entityManager.removeTransformComponent(entity);
entityManager.removeCollisionComponent(entity);

// 销毁整个实体（包括所有组件）
entityManager.destroyEntity(entity);
```

## 依赖库

- **GLM** (OpenGL Mathematics): 用于图形和游戏开发的数学库

## 许可证

根据您的需要添加适当的许可证。
