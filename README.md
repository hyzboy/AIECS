# AIECS - Hybrid Architecture Game Engine

一个基于 C++20 的现代游戏引擎，采用混合架构设计：**Frostbite 风格的对象接口 + SOA 高性能后端**。

## 核心特点

- **混合架构** - 结合 Frostbite 的易用性和 ECS 的性能
- **对象系统** - 统一的 Object 基类，完整的生命周期管理
- **模块化设计** - Module 系统支持独立的引擎子系统
- **事件驱动** - 完整的 EventSystem 支持发布-订阅模式
- **SOA 后端** - Transform 等关键组件使用 SOA 存储，性能提升 **47-60 倍**
- **灵活组件** - 动态组件管理，支持任意组件组合
- **层级系统** - 完整的父子关系和变换继承
- **C++20** - 使用现代 C++ 标准
- **GLM 数学库** - 用于向量、矩阵和四元数运算

## 性能优势

| 操作 | 时间 | 用途 |
|------|------|------|
| 创建 10000 实体 | 3 ms | 快速初始化 |
| 组件接口访问 | 714 µs | 一般游戏逻辑 |
| SOA 批处理 | 15 µs | 物理、渲染等 |
| **性能提升** | **47-60 倍** | 🚀 |

## 架构说明

### 核心系统

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
   - 管理所有游戏对象和模块
   - 统一更新入口

5. **GameEntity** (`include/GameEntity.h`)
   - 游戏对象实体
   - 动态组件管理（使用 hash_code 优化，50-60% 性能提升）
   - 模板化组件访问

### 混合组件系统

**TransformComponentFB** - 混合架构示例：
- **OOP 接口**：setPosition/getPosition 等，易用性
- **SOA 后端**：TransformDataStorage，47-60 倍批处理性能
- **自动同步**：Handle 系统透明管理数据映射
- **层级支持**：父子关系和世界变换

**其他组件**：
- **CollisionComponentFB** - 碰撞数据（可扩展 SOA）
- **RenderComponentFB** - 渲染数据（可扩展 SOA）


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

## 使用示例

### 基本用法 - 创建实体和组件

```cpp
#include "World.h"
#include "GameEntity.h"
#include "TransformComponentFB.h"

int main() {
    // 创建世界
    auto world = std::make_shared<World>("MainWorld");
    world->onCreate();
    
    // 创建游戏实体
    auto entity = std::make_shared<GameEntity>("Player");
    world->addObject(entity);
    
    // 添加 Transform 组件（OOP 接口）
    auto transform = entity->addComponent<TransformComponentFB>();
    transform->setPosition(glm::vec3(1.0f, 2.0f, 3.0f));
    transform->setRotation(glm::quat(glm::vec3(0, glm::radians(45.0f), 0)));
    
    // 更新世界
    world->onUpdate(0.016f);
    
    return 0;
}
```

### 高性能批处理 - SOA 后端访问

```cpp
// 创建大量实体
for (int i = 0; i < 10000; i++) {
    auto entity = std::make_shared<GameEntity>("Entity_" + std::to_string(i));
    world->addObject(entity);
    auto transform = entity->addComponent<TransformComponentFB>();
    transform->setPosition(glm::vec3(i * 1.0f, 0, 0));
}

// 批量处理（47-60x 性能提升）
auto storage = TransformComponentFB::getSharedStorage();
auto& positions = storage->getAllPositions();
for (auto& pos : positions) {
    pos.y += deltaTime * 9.8f; // 重力模拟
}
```

### 组件层级关系

```cpp
// 创建父子关系
auto parent = std::make_shared<GameEntity>("Parent");
auto child = std::make_shared<GameEntity>("Child");
world->addObject(parent);
world->addObject(child);

auto parentTransform = parent->addComponent<TransformComponentFB>();
auto childTransform = child->addComponent<TransformComponentFB>();

// 设置父子关系
childTransform->setParent(parentTransform.get());

// 父级变换自动影响子级
parentTransform->setPosition(glm::vec3(10, 0, 0));
childTransform->setLocalPosition(glm::vec3(5, 0, 0));

// 子级世界坐标为 (15, 0, 0)
glm::vec3 worldPos = childTransform->getWorldPosition();
```

### 事件系统

```cpp
#include "EventSystem.h"

// 创建事件系统
auto eventSystem = std::make_shared<EventSystem>();

// 订阅事件
int listenerId = eventSystem->subscribe("PlayerDied", [](const Event& event) {
    std::cout << "Player died!" << std::endl;
});

// 发送事件
Event event;
event.name = "PlayerDied";
eventSystem->send(event);

// 取消订阅
eventSystem->unsubscribe("PlayerDied", listenerId);
```

## 项目结构

```
AIECS/
├── CMakeLists.txt                  # CMake 配置
├── vcpkg.json                      # 依赖管理
├── include/                        # 头文件
│   ├── Object.h                    # 基础对象系统
│   ├── Module.h                    # 模块系统
│   ├── EventSystem.h               # 事件系统
│   ├── World.h                     # 世界管理
│   ├── GameEntity.h                # 游戏实体
│   ├── TransformComponentFB.h      # Transform 组件（混合架构）
│   ├── TransformDataStorage.h      # Transform SOA 后端
│   ├── CollisionComponentFB.h      # 碰撞组件
│   └── RenderComponentFB.h         # 渲染组件
├── src/
│   ├── Object.cpp
│   ├── Module.cpp
│   ├── EventSystem.cpp
│   ├── World.cpp
│   ├── GameEntity.cpp
│   ├── TransformComponentFB.cpp
│   ├── CollisionComponentFB.cpp
│   ├── RenderComponentFB.cpp
│   └── main.cpp                    # 混合架构演示
└── README.md
```

## 文档

- **HYBRID_ARCHITECTURE.md** - 混合架构详细设计
- **ARCHITECTURE_SUMMARY.md** - 三种架构对比
- **TRANSFORM_OPTIMIZATION.md** - Transform 优化技术
- **HYBRID_QUICK_START.md** - 快速开始指南
- **MIGRATION_GUIDE.md** - 迁移指南
- **OPTIMIZATION_GUIDE.md** - 性能优化指南

## 依赖库

- **GLM** (OpenGL Mathematics): 数学库（向量、矩阵、四元数）
- **C++20**: 现代 C++ 标准

## 许可证

MIT License
```
