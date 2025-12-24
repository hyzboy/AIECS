# AIECS Frostbite 架构 - 文档索引

## 📚 文档导航

### 入门指南
👉 **从这里开始！** 

1. **[README.md](README.md)** - 项目总览
   - 项目特点
   - 架构说明
   - 基本信息

2. **[QUICK_REFERENCE.md](QUICK_REFERENCE.md)** - 快速参考卡
   - 基本用法
   - 常用模式
   - 代码示例

### 详细文档

3. **[MIGRATION_GUIDE.md](MIGRATION_GUIDE.md)** - 迁移指南
   - 新架构组件详解
   - 迁移步骤
   - 主要改进
   - 扩展建议
   - 常见问题

4. **[ARCHITECTURE_COMPARISON.md](ARCHITECTURE_COMPARISON.md)** - 架构对比
   - ECS vs Frostbite 对比
   - 性能分析
   - 迁移检查清单
   - 过渡策略
   - 性能优化建议

### 完成报告

5. **[MIGRATION_COMPLETE.md](MIGRATION_COMPLETE.md)** - 迁移完成报告
   - 迁移概览
   - 新增文件清单
   - 核心架构详解
   - 编译和运行结果
   - 下一步建议

## 🎯 按用途查找

### 我想...

#### ...快速上手
→ [QUICK_REFERENCE.md](QUICK_REFERENCE.md) 的"基本用法"部分

#### ...理解整个架构
→ [MIGRATION_GUIDE.md](MIGRATION_GUIDE.md) 的"新的架构组件"和"主要改进"

#### ...从旧代码迁移
→ [ARCHITECTURE_COMPARISON.md](ARCHITECTURE_COMPARISON.md) 的"迁移检查清单"

#### ...创建自定义模块
→ [QUICK_REFERENCE.md](QUICK_REFERENCE.md) 的"创建自定义模块"和"常用模式"

#### ...了解性能特性
→ [ARCHITECTURE_COMPARISON.md](ARCHITECTURE_COMPARISON.md) 的"性能对比"

#### ...查看完整代码示例
→ [QUICK_REFERENCE.md](QUICK_REFERENCE.md) 的"常用模式"和"批量创建对象"

#### ...调试问题
→ [QUICK_REFERENCE.md](QUICK_REFERENCE.md) 的"调试技巧"和"常见错误"

#### ...看迁移前后对比
→ [ARCHITECTURE_COMPARISON.md](ARCHITECTURE_COMPARISON.md) 的"关键差异"

## 📂 代码位置

### 核心类
| 类 | 头文件 | 实现文件 | 用途 |
|-----|--------|---------|------|
| Object | include/Object.h | src/Object.cpp | 基础对象类 |
| Module | include/Module.h | src/Module.cpp | 模块基类 |
| EventSystem | include/EventSystem.h | src/EventSystem.cpp | 事件系统 |
| World | include/World.h | src/World.cpp | 世界管理 |
| GameEntity | include/GameEntity.h | src/GameEntity.cpp | 游戏对象 |
| TransformComponentFB | include/TransformComponentFB.h | src/TransformComponentFB.cpp | 变换组件 |
| CollisionComponentFB | include/CollisionComponentFB.h | src/CollisionComponentFB.cpp | 碰撞组件 |
| RenderComponentFB | include/RenderComponentFB.h | src/RenderComponentFB.cpp | 渲染组件 |

### 演示程序
| 程序 | 位置 | 描述 |
|------|------|------|
| Frostbite 演示 | src/main_frostbite.cpp | 新架构演示 |
| 原始 ECS 演示 | src/main.cpp | 原始架构演示 |

## 🔍 快速查找

### 按主题

#### 对象管理
- 创建对象 → [QUICK_REFERENCE.md#创建对象和组件](QUICK_REFERENCE.md)
- 对象生命周期 → [MIGRATION_GUIDE.md#生命周期管理](MIGRATION_GUIDE.md)
- Object 类详解 → [MIGRATION_COMPLETE.md#1-object---基础对象系统](MIGRATION_COMPLETE.md)

#### 组件系统
- 添加/移除组件 → [QUICK_REFERENCE.md#创建对象和组件](QUICK_REFERENCE.md)
- 自定义组件 → [QUICK_REFERENCE.md#创建自定义组件](QUICK_REFERENCE.md)
- Transform 操作 → [QUICK_REFERENCE.md#transform-操作](QUICK_REFERENCE.md)

#### 事件系统
- 订阅事件 → [QUICK_REFERENCE.md#事件系统](QUICK_REFERENCE.md)
- 自定义事件 → [QUICK_REFERENCE.md#创建自定义事件](QUICK_REFERENCE.md)
- EventSystem 详解 → [MIGRATION_COMPLETE.md#3-eventsystem---事件系统](MIGRATION_COMPLETE.md)

#### 模块系统
- 创建模块 → [QUICK_REFERENCE.md#创建自定义模块](QUICK_REFERENCE.md)
- Module 基类 → [MIGRATION_GUIDE.md#2-module-系统](MIGRATION_GUIDE.md)
- 自定义模块示例 → [QUICK_REFERENCE.md#常用模式](QUICK_REFERENCE.md)

#### 调试和优化
- 调试技巧 → [QUICK_REFERENCE.md#调试技巧](QUICK_REFERENCE.md)
- 常见错误 → [QUICK_REFERENCE.md#常见错误](QUICK_REFERENCE.md)
- 性能优化 → [QUICK_REFERENCE.md#性能建议](QUICK_REFERENCE.md)
- 性能对比 → [ARCHITECTURE_COMPARISON.md#性能对比](ARCHITECTURE_COMPARISON.md)

## 📊 版本对比

### ECS 版本
位置：`src/main.cpp`
特点：
- 高性能 SOA 数据布局
- 缓存友好
- 复杂的数据管理

### Frostbite 版本
位置：`src/main_frostbite.cpp`
特点：
- 对象导向
- 灵活的组件系统
- 事件驱动
- 推荐使用

## 🚀 快速开始

```cpp
// 1. 创建世界
auto world = std::make_shared<World>("MyGame");
world->initialize();

// 2. 创建对象
auto player = world->createObject<GameEntity>("Player");

// 3. 添加组件
auto transform = player->addComponent<TransformComponentFB>();
auto render = player->addComponent<RenderComponentFB>();

// 4. 配置
transform->setLocalPosition(glm::vec3(0, 1, 0));
render->setMeshName("player.mesh");

// 5. 更新循环
while (running) {
    world->update(deltaTime);
}

// 6. 清理
world->shutdown();
```

## 📖 学习路径

### 完全初学者
1. 阅读 [README.md](README.md)
2. 查看 [QUICK_REFERENCE.md](QUICK_REFERENCE.md) 的"基本用法"
3. 运行 `aiecs_frostbite.exe` 查看演示
4. 参考代码修改 `main_frostbite.cpp`

### 有 ECS 经验的开发者
1. 阅读 [ARCHITECTURE_COMPARISON.md](ARCHITECTURE_COMPARISON.md)
2. 查看 [MIGRATION_GUIDE.md](MIGRATION_GUIDE.md) 的"迁移步骤"
3. 使用 [QUICK_REFERENCE.md](QUICK_REFERENCE.md) 快速转换

### 想深入理解的
1. 阅读所有文档
2. 检查源代码注释
3. 运行两个演示程序对比
4. 参考 [MIGRATION_COMPLETE.md](MIGRATION_COMPLETE.md) 了解详细实现

## ❓ FAQ 速查

| 问题 | 答案位置 |
|------|---------|
| 如何创建对象? | [QUICK_REFERENCE.md#创建对象和组件](QUICK_REFERENCE.md) |
| 如何添加组件? | [QUICK_REFERENCE.md#创建对象和组件](QUICK_REFERENCE.md) |
| 如何使用事件? | [QUICK_REFERENCE.md#事件系统](QUICK_REFERENCE.md) |
| 如何创建模块? | [QUICK_REFERENCE.md#创建自定义模块](QUICK_REFERENCE.md) |
| 性能如何? | [ARCHITECTURE_COMPARISON.md#性能对比](ARCHITECTURE_COMPARISON.md) |
| 如何从 ECS 迁移? | [ARCHITECTURE_COMPARISON.md#迁移检查清单](ARCHITECTURE_COMPARISON.md) |
| 有什么限制? | [MIGRATION_COMPLETE.md#已知限制和注意事项](MIGRATION_COMPLETE.md) |

## 🔗 快速链接

### 关键类和类型
- Object 基类 - include/Object.h
- Module 基类 - include/Module.h  
- World 管理器 - include/World.h
- GameEntity 对象 - include/GameEntity.h
- TransformComponentFB - include/TransformComponentFB.h
- CollisionComponentFB - include/CollisionComponentFB.h
- RenderComponentFB - include/RenderComponentFB.h

### CMake 配置
- 编译脚本 - CMakeLists.txt
- vcpkg 配置 - vcpkg.json
- vcpkg 配置详情 - vcpkg-configuration.json

### 可执行文件
- Frostbite 演示 - bin/Release/aiecs_frostbite.exe
- ECS 演示 - bin/Release/aiecs_original.exe

## 📝 文档统计

| 文档 | 行数 | 内容 |
|------|------|------|
| README.md | ~278 | 项目概览 |
| QUICK_REFERENCE.md | ~421 | 快速参考 |
| MIGRATION_GUIDE.md | ~303 | 迁移指南 |
| ARCHITECTURE_COMPARISON.md | ~364 | 架构对比 |
| MIGRATION_COMPLETE.md | ~462 | 完成报告 |
| **总计** | **~1828** | **完整文档** |

## 🎓 推荐学习顺序

1. **Day 1**: 阅读 README 和 QUICK_REFERENCE
2. **Day 2**: 运行演示程序，修改代码实验
3. **Day 3**: 阅读 MIGRATION_GUIDE，了解详细架构
4. **Day 4**: 查看 ARCHITECTURE_COMPARISON，对比设计
5. **Day 5+**: 创建自己的项目，参考文档和示例

---

**选择一个文档开始阅读吧！** 📖

或者直接运行演示程序：
```bash
bin/Release/aiecs_frostbite.exe
```
