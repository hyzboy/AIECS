# Frostbite 架构 - 快速参考卡

## 基本用法

### 创建世界
```cpp
auto world = std::make_shared<World>("MyWorld");
world->initialize();
// ... 使用world
world->shutdown();
```

### 创建对象和组件
```cpp
// 创建对象
auto entity = world->createObject<GameEntity>("MyEntity");

// 添加组件
auto transform = entity->addComponent<TransformComponentFB>();
auto render = entity->addComponent<RenderComponentFB>();

// 访问组件
if (auto t = entity->getComponent<TransformComponentFB>()) {
    t->setLocalPosition(glm::vec3(0, 1, 0));
}

// 检查组件
if (entity->hasComponent<TransformComponentFB>()) {
    // ...
}

// 移除组件
entity->removeComponent<CollisionComponentFB>();
```

### 事件系统
```cpp
auto events = world->getEventSystem();

// 订阅
events->subscribe("MyEvent", [](auto& evt) {
    // 处理事件
});

// 分发（同步）
events->dispatch(std::make_shared<MyEvent>());

// 队列（异步）
events->queue(std::make_shared<MyEvent>());
```

### 创建自定义模块
```cpp
class MyModule : public Module {
public:
    void initialize() override { 
        // 初始化
    }
    void update(float dt) override { 
        // 每帧更新
    }
    void shutdown() override { 
        // 清理
    }
};

auto myModule = world->registerModule<MyModule>();
```

### Transform 操作
```cpp
auto t = entity->getComponent<TransformComponentFB>();

// 本地变换
t->setLocalPosition(glm::vec3(1, 2, 3));
t->setLocalRotation(glm::quat(1, 0, 0, 0));
t->setLocalScale(glm::vec3(2, 2, 2));
t->setLocalTRS(pos, rot, scale);

// 世界变换
glm::vec3 worldPos = t->getWorldPosition();
glm::quat worldRot = t->getWorldRotation();
glm::vec3 worldScale = t->getWorldScale();

// 矩阵
glm::mat4 localMat = t->getLocalMatrix();
glm::mat4 worldMat = t->getWorldMatrix();

// 父子关系
t->setParent(parentEntity);
t->addChild(childEntity);
t->removeChild(childEntity);
```

### Collision 操作
```cpp
auto c = entity->getComponent<CollisionComponentFB>();

c->setBoundingBox(glm::vec3(-1), glm::vec3(1));
glm::vec3 bbMin = c->getBoundingBoxMin();
glm::vec3 bbMax = c->getBoundingBoxMax();

c->setCollisionLayer(1);
uint32_t layer = c->getCollisionLayer();

c->setEnabled(true);
bool enabled = c->isEnabled();
```

### Render 操作
```cpp
auto r = entity->getComponent<RenderComponentFB>();

r->setMeshName("mesh.mesh");
r->setMaterialName("material.mat");
r->setVisible(true);
r->setCastShadows(true);
```

## 常用模式

### 创建自定义事件
```cpp
struct MyCustomEvent : public EventData {
    MyCustomEvent() : EventData("MyCustomEvent") {}
    // 自定义数据
    std::string message;
};
```

### 创建自定义组件
```cpp
class MyComponent : public Component {
public:
    void onAttach() override { }
    void onUpdate(float dt) override { }
    void onDetach() override { }
};
```

### 批量创建对象
```cpp
std::vector<std::shared_ptr<GameEntity>> entities;
for (int i = 0; i < 100; i++) {
    auto e = world->createObject<GameEntity>("Entity_" + std::to_string(i));
    e->addComponent<TransformComponentFB>();
    e->addComponent<RenderComponentFB>();
    entities.push_back(e);
}
```

### 层级关系设置
```cpp
auto parent = world->createObject<GameEntity>("Parent");
auto parentT = parent->addComponent<TransformComponentFB>();

auto child1 = world->createObject<GameEntity>("Child1");
auto child1T = child1->addComponent<TransformComponentFB>();
child1T->setParent(parent);
parentT->addChild(child1);

auto child2 = world->createObject<GameEntity>("Child2");
auto child2T = child2->addComponent<TransformComponentFB>();
child2T->setParent(parent);
parentT->addChild(child2);
```

### 事件驱动的交互
```cpp
// 定义事件
struct CollisionEvent : public EventData {
    CollisionEvent(GameEntity* a, GameEntity* b) 
        : EventData("Collision"), entityA(a), entityB(b) {}
    GameEntity* entityA;
    GameEntity* entityB;
};

// 监听事件
auto events = world->getEventSystem();
events->subscribe("Collision", [](auto& evt) {
    auto colEvent = std::dynamic_pointer_cast<CollisionEvent>(evt);
    // 处理碰撞
});

// 触发事件
events->dispatch(std::make_shared<CollisionEvent>(e1, e2));
```

## 对象生命周期

```
GameEntity 创建
    ↓
Object::onCreate() 调用
    ↓
组件附加（onAttach）
    ↓
每帧更新（onUpdate）
    ↓
组件移除（onDetach）
    ↓
Object::onDestroy() 调用
    ↓
对象销毁
```

## 常见类型

```cpp
// 对象ID
Object::ObjectID id = entity->getID();

// 组件
std::shared_ptr<TransformComponentFB> t = 
    entity->getComponent<TransformComponentFB>();

// 事件
std::shared_ptr<EventData> event = 
    std::make_shared<MyEvent>();

// 模块
std::shared_ptr<MyModule> mod = 
    world->getModule<MyModule>();
```

## 调试技巧

```cpp
// 检查对象有效性
if (entity && entity->isValid()) {
    // ...
}

// 获取对象信息
std::cout << "Object ID: " << entity->getID() << std::endl;
std::cout << "Object Name: " << entity->getName() << std::endl;
std::cout << "Component Count: " << entity->getComponentCount() << std::endl;

// 检查世界状态
std::cout << "Objects: " << world->getObjectCount() << std::endl;
std::cout << "World Active: " << (world->isActive() ? "Yes" : "No") << std::endl;

// 检查模块
auto eventSys = world->getEventSystem();
if (eventSys && eventSys->isInitialized()) {
    // ...
}
```

## 性能建议

1. ✅ 使用对象池减少分配
2. ✅ 批量操作相同类型的组件
3. ✅ 使用异步事件（queue）处理频繁事件
4. ✅ 避免在 onUpdate 中频繁查找组件
5. ✅ 正确管理对象生命周期

## 常见错误

❌ 不初始化 World
```cpp
auto world = std::make_shared<World>();
world->createObject<GameEntity>();  // 错误！需要先初始化
```

✅ 正确做法
```cpp
auto world = std::make_shared<World>();
world->initialize();
world->createObject<GameEntity>();
```

---

❌ 不检查组件有效性
```cpp
entity->getComponent<TransformComponentFB>()->setLocalPosition(pos);  // 可能崩溃
```

✅ 正确做法
```cpp
if (auto t = entity->getComponent<TransformComponentFB>()) {
    t->setLocalPosition(pos);
}
```

---

❌ 尝试访问无效的对象
```cpp
entity->addComponent<RenderComponentFB>();  // 如果entity无效可能崩溃
```

✅ 正确做法
```cpp
if (entity && entity->isValid()) {
    entity->addComponent<RenderComponentFB>();
}
```

## 更多信息

- 详细文档：`MIGRATION_GUIDE.md`
- 架构对比：`ARCHITECTURE_COMPARISON.md`
- 完成报告：`MIGRATION_COMPLETE.md`
- 源代码注释：各头文件中的详细说明
