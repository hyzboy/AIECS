# AIECS

一个基于 C++20 和 CMake 的 ECS (Entity Component System) 项目，使用 vcpkg 进行依赖管理，并集成 GLM 数学库。

## 项目特点

- **纯组件 ECS 架构**: 真正的实体组件系统，组件之间完全独立，无继承关系
- **SOA 存储**: 所有组件使用 Structure of Arrays 模式，提高缓存性能
- **C++20**: 使用现代 C++ 标准
- **GLM 数学库**: 用于向量、矩阵和四元数运算
- **Vulkan 目标**: 设计用于 Vulkan 图形API
- **坐标系统**: 右手坐标系，Z 轴向上
- **深度范围**: 0-1（Vulkan 标准）

## 架构说明

### 核心组件

1. **EntityContext** (`include/EntityContext.h`)
   - 存储系统的上下文结构
   - 包含指向所有组件存储系统的指针
   - TransformStorage、CollisionStorage、RenderStorage 等
   - 通过 EntityManager 初始化并传递给 Entity

2. **Entity** (`include/Entity.h`)
   - 轻量级实体句柄，仅存储组件索引
   - 不包含任何组件数据或方法
   - 通过 getComponent() 方法访问组件
   - 返回 std::optional<Component> 用于安全访问
   - 构造函数为私有，只能通过 EntityManager 创建

3. **组件存储系统 (SOA 模式)**
   - **TransformStorage** - 位置、旋转、缩放、世界矩阵、父子关系
   - **CollisionStorage** - 包围盒、碰撞层、启用状态
   - **RenderStorage** - 网格名、材质名、可见性、阴影投射

4. **组件访问器**
   - **TransformComponent** - Transform/TRS 访问器
     - 本地变换（相对父级）：getLocal/setLocal
     - 世界变换（绝对坐标）：getWorld/setWorld
     - 世界变换设置时自动反推本地 TRS
   - **CollisionComponent** - 碰撞数据访问器
     - 包围盒、碰撞层、启用/禁用
   - **RenderComponent** - 渲染数据访问器
     - 网格、材质、可见性、阴影

5. **EntityManager** (`include/EntityManager.h`)
   - 管理实体和所有组件的创建与销毁
   - 添加/移除组件：addComponent/removeComponent
   - 内置所有组件存储系统
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
