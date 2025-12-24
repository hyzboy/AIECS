# 架构对比与迁移说明

## 关键差异

### 1. 对象模型

**原始 ECS:**
```cpp
// 仅包含组件索引的轻量级句柄
class Entity {
    uint32_t entityId;
    ComponentID transformComponentId;
    ComponentID collisionComponentId;
    ComponentID renderComponentId;
};
```

**Frostbite:**
```cpp
// 真实对象，可以包含组件
class GameEntity : public Object {
    std::unordered_map<std::string, std::shared_ptr<Component>> components;
};
```

### 2. 系统管理

**原始 ECS:**
```cpp
// 单一管理器
EntityManager entityManager;
entityManager.addTransformComponent(entity);
entityManager.updateAllTransforms();
```

**Frostbite:**
```cpp
// 模块化系统
auto world = std::make_shared<World>();
auto entity = world->createObject<GameEntity>();
entity->addComponent<TransformComponentFB>();
world->update(deltaTime);
```

### 3. 组件访问

**原始 ECS:**
```cpp
auto transform = entity.getTransformComponent();
if (transform) {
    transform->setLocalPosition(pos);
}
```

**Frostbite:**
```cpp
auto transform = entity->getComponent<TransformComponentFB>();
if (transform) {
    transform->setLocalPosition(pos);
}
```

### 4. 事件通信

**原始 ECS:**
```cpp
// 无事件系统
// 需要直接函数调用
```

**Frostbite:**
```cpp
// 事件驱动
auto events = world->getEventSystem();
events->subscribe("EntityCreated", callback);
events->dispatch(event);
```

### 5. 数据存储

**原始 ECS - SOA (Structure of Arrays):**
```cpp
// 相同类型的数据连续存储
struct TransformStorage {
    std::vector<glm::vec3> positions;
    std::vector<glm::quat> rotations;
    std::vector<glm::vec3> scales;
};
```

**Frostbite - 面向对象:**
```cpp
// 每个对象独立存储其组件
class GameEntity {
    std::unordered_map<std::string, Component*> components;
};
```

## 性能对比

| 方面 | 原始 ECS | Frostbite |
|------|---------|-----------|
| **缓存友好** | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ |
| **内存使用** | 紧凑 | 稍多（指针开销） |
| **访问速度** | 非常快（缓存命中） | 中等（随机访问） |
| **灵活性** | 固定组件 | 动态组件 |
| **易用性** | 较复杂 | 更直观 |
| **可扩展性** | 需要修改存储 | 简单添加新组件 |
| **并行化** | 天然友好 | 需要注意 |

## 迁移检查表

### 代码迁移

- [ ] 将 `Entity` 改为 `GameEntity`
- [ ] 删除 `EntityManager` 引用，使用 `World`
- [ ] 更新组件类型：
  - `TransformComponent` → `TransformComponentFB`
  - `CollisionComponent` → `CollisionComponentFB`
  - `RenderComponent` → `RenderComponentFB`
- [ ] 更新创建方式：
  ```cpp
  // 旧
  Entity e = manager.createEntity();
  manager.addTransformComponent(e);
  
  // 新
  auto e = world->createObject<GameEntity>();
  e->addComponent<TransformComponentFB>();
  ```
- [ ] 实现自定义事件
- [ ] 创建必要的模块

### 系统集成

- [ ] 创建 World 实例
- [ ] 注册必要的模块
- [ ] 设置事件监听
- [ ] 实现游戏循环

```cpp
auto world = std::make_shared<World>();
world->initialize();

while (running) {
    world->update(deltaTime);
}

world->shutdown();
```

## 何时使用哪个版本？

### 使用原始 ECS 如果：
- 需要高性能、缓存友好的系统
- 构建大规模粒子系统或 SIMD 密集应用
- 对象之间交互很少
- 需要最大化吞吐量

### 使用 Frostbite 如果：
- 需要灵活的对象系统
- 需要事件驱动架构
- 需要模块化的系统管理
- 对象需要复杂的层级关系
- 优先考虑易用性和可维护性
- 构建完整的游戏引擎

## 过渡策略

### 方案 1: 渐进式迁移
1. 保留两个版本
2. 新代码使用 Frostbite
3. 逐步迁移旧代码
4. 最终完全弃用 ECS 版本

### 方案 2: 包装器模式
创建适配层，使 ECS 代码可以在 Frostbite 中运行：

```cpp
class LegacyEntityAdapter : public GameEntity {
    EntityManager& legacyManager;
    Entity legacyEntity;
    
public:
    void addComponent<T>() {
        // 桥接到旧系统
    }
};
```

### 方案 3: 共存
在同一项目中使用两个系统：
- Frostbite 用于高级游戏逻辑
- ECS 用于性能关键的系统（粒子、物理等）

## 扩展性对比

### 原始 ECS - 添加新组件
1. 创建新的 Storage 类
2. 创建新的 Component 访问器
3. 修改 EntityContext
4. 修改 EntityManager
5. 修改 Entity

**工作量:** 很大

### Frostbite - 添加新组件
1. 创建新的 Component 子类
2. 完成！

**工作量:** 最小

## 性能优化建议

### Frostbite 版本
1. **使用对象池**
```cpp
class GameEntityPool {
    std::vector<std::shared_ptr<GameEntity>> pool;
    std::queue<std::shared_ptr<GameEntity>> available;
};
```

2. **批处理更新**
```cpp
// 按组件类型批处理
world->forEachEntity<TransformComponentFB>([](auto& t) {
    // 批量处理
});
```

3. **缓存优化**
- 使用 `std::vector` 代替 `std::unordered_map` 存储热数据
- 数据局部性优化

4. **事件优化**
- 使用事件队列而不是同步分发
- 合并相同类型的事件

## 总结

| 特性 | ECS | Frostbite |
|------|-----|-----------|
| 学习曲线 | 陡峭 | 平缓 |
| 性能天花板 | 非常高 | 高 |
| 扩展难度 | 高 | 低 |
| 代码复杂度 | 高 | 中等 |
| 适合新手 | 否 | 是 |
| 适合AAA游戏 | 是 | 是 |
| 适合小型项目 | 否 | 是 |

## 参考资源

- [Frostbite 引擎架构](https://www.gdcvault.com/play/1022038/)
- [数据导向设计](https://www.dataorienteddesign.com/)
- [ECS vs OOP 对比](https://www.gamedev.net/)
