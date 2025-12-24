# Frostbite Architecture Migration Guide

## 概述
本项目已成功迁移到Frostbite引擎的架构模式。Frostbite是EA开发的先进游戏引擎，采用了现代化的对象和模块系统。

## 新的架构组件

### 1. Object 基类 (`Object.h / Object.cpp`)
所有游戏对象的基类，提供：
- 唯一的对象ID
- 对象名称管理
- 生命周期回调（onCreate, onUpdate, onDestroy）
- 对象有效性检查

### 2. Module 系统 (`Module.h / Module.cpp`)
为单例式的引擎系统提供基类，包括：
- 初始化和关闭机制
- 每帧更新支持
- 模块状态跟踪

### 3. EventSystem (`EventSystem.h / EventSystem.cpp`)
事件驱动系统，支持：
- 事件订阅/取消订阅
- 同步和异步事件分发
- 事件队列处理

### 4. World (`World.h / World.cpp`)
游戏世界管理器，负责：
- 管理所有游戏对象
- 管理引擎模块
- 协调世界更新循环
- 提供对各模块的访问

### 5. GameEntity (`GameEntity.h / GameEntity.cpp`)
游戏对象实体，特点：
- 继承自Object
- 支持动态组件系统
- 模板化的组件管理

### 6. 组件系统
#### TransformComponentFB (`TransformComponentFB.h / TransformComponentFB.cpp`)
- 本地和世界坐标变换
- 父子关系管理
- 层级矩阵更新
- 支持位置、旋转、缩放

#### CollisionComponentFB (`CollisionComponentFB.h / CollisionComponentFB.cpp`)
- 碰撞边界框管理
- 碰撞层设置
- 碰撞启用/禁用

#### RenderComponentFB (`RenderComponentFB.h / RenderComponentFB.cpp`)
- 网格和材质管理
- 可见性控制
- 阴影投射设置

## 架构对比

### 旧架构 vs 新架构

| 方面 | 旧ECS | Frostbite风格 |
|------|-------|--------------|
| 对象基类 | - | Object |
| 系统管理 | EntityManager | Module + World |
| 事件系统 | 无 | EventSystem |
| 组件访问 | SOA存储 | 动态组件字典 |
| 层级管理 | 无 | 父子关系 |
| 回调系统 | 无 | onCreate/onUpdate/onDestroy |

## 迁移步骤

### 1. 从旧Entity迁移到GameEntity
```cpp
// 旧方式
Entity entity = entityManager.createEntity();
entityManager.addTransformComponent(entity);

// 新方式
auto entity = world->createObject<GameEntity>("MyEntity");
auto transform = entity->addComponent<TransformComponentFB>();
```

### 2. 组件访问方式
```cpp
// 旧方式
auto transform = entity.getTransformComponent();
if (transform) {
    transform->setLocalPosition(pos);
}

// 新方式
auto transform = entity->getComponent<TransformComponentFB>();
if (transform) {
    transform->setLocalPosition(pos);
}
```

### 3. 事件系统使用
```cpp
// 订阅事件
world->getEventSystem()->subscribe("EntityCreated", 
    [](const std::shared_ptr<EventData>& event) {
        // 处理事件
    });

// 分发事件
world->getEventSystem()->dispatch(
    std::make_shared<MyCustomEvent>());
```

### 4. 模块注册
```cpp
// 自定义模块必须继承Module
class MyModule : public Module {
public:
    void initialize() override { /* ... */ }
    void shutdown() override { /* ... */ }
    void update(float deltaTime) override { /* ... */ }
};

// 在World中注册
auto myModule = world->registerModule<MyModule>();
```

## 主要改进

### 1. 面向对象的对象系统
- 更符合传统游戏引擎设计
- 更容易理解和维护
- 支持继承和多态

### 2. 事件驱动架构
- 解耦系统之间的依赖
- 支持异步事件处理
- 易于扩展

### 3. 模块化设计
- 清晰的系统边界
- 模块独立初始化和更新
- 易于管理依赖关系

### 4. 层级系统
- 支持父子关系
- 自动世界坐标变换
- 符合大多数游戏引擎设计

### 5. 生命周期管理
- 明确的创建、更新、销毁阶段
- 自动资源清理
- 防止悬挂指针

## 编译和运行

### 编译所有目标
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### 运行原始版本
```bash
./bin/aiecs_original.exe
```

### 运行Frostbite版本
```bash
./bin/aiecs_frostbite.exe
```

## 性能考虑

### Frostbite架构的优势
1. **缓存友好** - 对象和组件的合理分配
2. **灵活性** - 动态组件系统
3. **可扩展性** - 模块系统易于添加新功能
4. **可维护性** - 清晰的结构和流程

### 注意事项
1. 避免过多的动态分配
2. 使用对象池优化频繁创建销毁
3. 模块更新顺序要合理安排
4. 事件队列容量监控

## 扩展建议

### 1. 添加更多模块
- 物理系统（PhysicsModule）
- 音频系统（AudioModule）
- 输入系统（InputModule）
- 渲染系统（RenderModule）

### 2. 自定义事件
```cpp
struct MyEvent : public EventData {
    MyEvent() : EventData("MyEvent") {}
    // 自定义数据
};
```

### 3. 自定义组件
```cpp
class MyComponent : public Component {
public:
    void onAttach() override { /* ... */ }
    void onUpdate(float deltaTime) override { /* ... */ }
    void onDetach() override { /* ... */ }
};
```

### 4. 自定义Module
```cpp
class MyModule : public Module {
public:
    void initialize() override { /* ... */ }
    void shutdown() override { /* ... */ }
    void update(float deltaTime) override { /* ... */ }
};
```

## 常见问题

### Q: 如何在组件间通信？
A: 使用事件系统。订阅和发送事件，实现解耦通信。

### Q: 如何创建自定义系统？
A: 继承Module类并实现initialize/shutdown/update方法，然后通过World注册。

### Q: 如何处理资源加载？
A: 建议创建ResourceModule来管理资源的异步加载和卸载。

### Q: 父子关系如何工作？
A: 子对象的世界坐标自动相对于父对象计算，移动父对象会自动移动所有子对象。

## 下一步

迁移完成后，建议：
1. 添加更多引擎模块（物理、音频、渲染）
2. 实现对象序列化/反序列化
3. 添加编辑器集成
4. 优化性能（对象池、批处理等）
5. 编写完整的文档和示例
