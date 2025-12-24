#include <iostream>
#include <chrono>
#include <glm/glm.hpp>
#include "World.h"
#include "GameEntity.h"
#include "TransformComponent.h"
#include "TransformDataStorage.h"
#include "CollisionComponent.h"
#include "CollisionDataStorage.h"

void demonstrateSOABatchProcessing() {
    std::cout << "\n=== SOA Batch Processing Demonstration ===" << std::endl;
    std::cout << std::endl;

    // Create a world
    auto world = std::make_shared<World>("BatchWorld");
    world->initialize();

    // Create many entities with transforms
    const int ENTITY_COUNT = 10000;
    std::vector<std::shared_ptr<GameEntity>> entities;
    
    std::cout << "Creating " << ENTITY_COUNT << " entities with transforms..." << std::endl;
    auto startCreate = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < ENTITY_COUNT; ++i) {
        auto entity = world->createObject<GameEntity>("Entity_" + std::to_string(i));
        auto transform = entity->addComponent<TransformComponent>();
        
        // Initialize with some data
        transform->setLocalPosition(glm::vec3(i * 0.1f, i * 0.2f, i * 0.3f));
        transform->setLocalScale(glm::vec3(1.0f + i * 0.001f));
        
        entities.push_back(entity);
    }
    
    auto endCreate = std::chrono::high_resolution_clock::now();
    auto createDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endCreate - startCreate);
    std::cout << "Created in " << createDuration.count() << " ms" << std::endl << std::endl;

    // Demonstrate 1: Sequential access (still needed for component interface)
    std::cout << "--- Sequential Access (Component Interface) ---" << std::endl;
    auto startSeq = std::chrono::high_resolution_clock::now();
    
    volatile float sum = 0.0f;
    for (const auto& entity : entities) {
        auto transform = entity->getComponent<TransformComponent>();
        if (transform) {
            glm::vec3 pos = transform->getLocalPosition();
            sum += pos.x + pos.y + pos.z;  // Do some work
        }
    }
    
    auto endSeq = std::chrono::high_resolution_clock::now();
    auto seqDuration = std::chrono::duration_cast<std::chrono::microseconds>(endSeq - startSeq);
    std::cout << "Sequential access: " << seqDuration.count() << " µs" << std::endl << std::endl;

    // Demonstrate 2: Batch processing with SOA (much faster!)
    std::cout << "--- Batch Processing (SOA Backend) ---" << std::endl;
    auto startBatch = std::chrono::high_resolution_clock::now();
    
    auto storage = TransformComponent::getSharedStorage();
    const auto& positions = storage->getAllPositions();
    
    // Direct array access - super cache friendly!
    volatile float batchSum = 0.0f;
    for (const auto& pos : positions) {
        batchSum += pos.x + pos.y + pos.z;  // Same work, but cache efficient
    }
    
    auto endBatch = std::chrono::high_resolution_clock::now();
    auto batchDuration = std::chrono::duration_cast<std::chrono::microseconds>(endBatch - startBatch);
    std::cout << "Batch processing: " << batchDuration.count() << " µs" << std::endl;
    
    if (seqDuration.count() > 0) {
        float speedup = static_cast<float>(seqDuration.count()) / static_cast<float>(batchDuration.count());
        std::cout << "Speedup: " << speedup << "x faster" << std::endl;
    }
    std::cout << std::endl;

    // Demonstrate 3: Update all positions efficiently
    std::cout << "--- Batch Update Operations ---" << std::endl;
    auto startUpdate = std::chrono::high_resolution_clock::now();
    
    // Update all positions with a single callback
    storage->updateAllDirtyMatrices([](TransformDataStorage::HandleID id, 
                                       glm::vec3 pos, 
                                       glm::quat rot, 
                                       glm::vec3 scale) {
        // This processes all dirty transforms at once, very cache friendly
        // In reality, you'd compute world matrices here for all at once
    });
    
    auto endUpdate = std::chrono::high_resolution_clock::now();
    auto updateDuration = std::chrono::duration_cast<std::chrono::microseconds>(endUpdate - startUpdate);
    std::cout << "Batch update: " << updateDuration.count() << " µs" << std::endl << std::endl;

    // Demonstrate 4: Direct access to all data for physics/rendering
    std::cout << "--- Direct Access to SOA Arrays ---" << std::endl;
    std::cout << "Accessing position array directly:" << std::endl;
    std::cout << "  Size: " << storage->getAllPositions().size() << " elements" << std::endl;
    std::cout << "  Memory: " << (storage->getAllPositions().size() * sizeof(glm::vec3) / 1024) << " KB" << std::endl;
    
    std::cout << "Accessing rotation array directly:" << std::endl;
    std::cout << "  Size: " << storage->getAllRotations().size() << " elements" << std::endl;
    std::cout << "  Memory: " << (storage->getAllRotations().size() * sizeof(glm::quat) / 1024) << " KB" << std::endl;
    
    std::cout << "Accessing scale array directly:" << std::endl;
    std::cout << "  Size: " << storage->getAllScales().size() << " elements" << std::endl;
    std::cout << "  Memory: " << (storage->getAllScales().size() * sizeof(glm::vec3) / 1024) << " KB" << std::endl;
    std::cout << std::endl;

    // Summary
    std::cout << "=== Summary ===" << std::endl;
    std::cout << "✓ Component interface: Easy, object-oriented, flexible" << std::endl;
    std::cout << "✓ SOA backend: Fast, cache-efficient, batch-friendly" << std::endl;
    std::cout << "✓ Best of both worlds: OOP API + SOA performance!" << std::endl;
    std::cout << std::endl;

    world->shutdown();
}

// 演示碰撞检测的 SOA 批处理
void demonstrateCollisionBatchProcessing() {
    std::cout << "\n=== Collision Detection SOA Batch Processing ===" << std::endl;
    std::cout << std::endl;

    auto world = std::make_shared<World>("CollisionWorld");
    world->initialize();

    const int ENTITY_COUNT = 10000;
    std::vector<std::shared_ptr<GameEntity>> entities;
    
    std::cout << "Creating " << ENTITY_COUNT << " entities with collision..." << std::endl;
    auto startCreate = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < ENTITY_COUNT; ++i) {
        auto entity = world->createObject<GameEntity>("Entity_" + std::to_string(i));
        auto collision = entity->addComponent<CollisionComponent>();
        
        // 设置不同大小的包围盒
        float size = 1.0f + (i % 10) * 0.5f;
        collision->setBoundingBox(
            glm::vec3(-size, -size, -size),
            glm::vec3(size, size, size)
        );
        collision->setCollisionLayer(i % 4);  // 4 个碰撞层
        
        entities.push_back(entity);
    }
    
    auto endCreate = std::chrono::high_resolution_clock::now();
    auto createDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endCreate - startCreate);
    std::cout << "Created in " << createDuration.count() << " ms" << std::endl << std::endl;

    // 方法 1: 传统的组件接口访问（慢）
    std::cout << "--- Method 1: Component Interface Access ---" << std::endl;
    auto startSeq = std::chrono::high_resolution_clock::now();
    
    int collisionCount = 0;
    for (size_t i = 0; i < entities.size(); ++i) {
        auto col1 = entities[i]->getComponent<CollisionComponent>();
        if (!col1 || !col1->isEnabled()) continue;
        
        auto min1 = col1->getBoundingBoxMin();
        auto max1 = col1->getBoundingBoxMax();
        
        // 简单的 AABB 碰撞检测（仅检测几个邻近的）
        for (size_t j = i + 1; j < std::min(i + 10, entities.size()); ++j) {
            auto col2 = entities[j]->getComponent<CollisionComponent>();
            if (!col2 || !col2->isEnabled()) continue;
            
            auto min2 = col2->getBoundingBoxMin();
            auto max2 = col2->getBoundingBoxMax();
            
            // AABB 碰撞检测
            bool overlap = (min1.x <= max2.x && max1.x >= min2.x) &&
                          (min1.y <= max2.y && max1.y >= min2.y) &&
                          (min1.z <= max2.z && max1.z >= min2.z);
            if (overlap) collisionCount++;
        }
    }
    
    auto endSeq = std::chrono::high_resolution_clock::now();
    auto seqDuration = std::chrono::duration_cast<std::chrono::microseconds>(endSeq - startSeq);
    std::cout << "Component interface: " << seqDuration.count() << " µs" << std::endl;
    std::cout << "Found " << collisionCount << " collisions" << std::endl << std::endl;

    // 方法 2: SOA 批处理（快！）
    std::cout << "--- Method 2: SOA Batch Processing ---" << std::endl;
    auto startBatch = std::chrono::high_resolution_clock::now();
    
    auto storage = CollisionComponent::getSharedStorage();
    const auto& mins = storage->getAllBoundingBoxMins();
    const auto& maxs = storage->getAllBoundingBoxMaxs();
    const auto& enabled = storage->getAllEnabledFlags();
    
    int batchCollisionCount = 0;
    // 直接访问数组 - 缓存友好！
    for (size_t i = 0; i < mins.size(); ++i) {
        if (!enabled[i]) continue;
        
        const auto& min1 = mins[i];
        const auto& max1 = maxs[i];
        
        for (size_t j = i + 1; j < std::min(i + 10, mins.size()); ++j) {
            if (!enabled[j]) continue;
            
            const auto& min2 = mins[j];
            const auto& max2 = maxs[j];
            
            // AABB 碰撞检测
            bool overlap = (min1.x <= max2.x && max1.x >= min2.x) &&
                          (min1.y <= max2.y && max1.y >= min2.y) &&
                          (min1.z <= max2.z && max1.z >= min2.z);
            if (overlap) batchCollisionCount++;
        }
    }
    
    auto endBatch = std::chrono::high_resolution_clock::now();
    auto batchDuration = std::chrono::duration_cast<std::chrono::microseconds>(endBatch - startBatch);
    std::cout << "SOA batch processing: " << batchDuration.count() << " µs" << std::endl;
    std::cout << "Found " << batchCollisionCount << " collisions" << std::endl;
    
    if (seqDuration.count() > 0) {
        float speedup = static_cast<float>(seqDuration.count()) / static_cast<float>(batchDuration.count());
        std::cout << "Speedup: " << speedup << "x faster! 🚀" << std::endl;
    }
    std::cout << std::endl;

    // 演示按碰撞层批处理
    std::cout << "--- Collision Layer Filtering (SOA) ---" << std::endl;
    const auto& layers = storage->getAllCollisionLayers();
    
    auto startLayer = std::chrono::high_resolution_clock::now();
    
    // 统计每层的碰撞体数量
    std::vector<int> layerCounts(4, 0);
    for (size_t i = 0; i < layers.size(); ++i) {
        if (enabled[i] && layers[i] < 4) {
            layerCounts[layers[i]]++;
        }
    }
    
    auto endLayer = std::chrono::high_resolution_clock::now();
    auto layerDuration = std::chrono::duration_cast<std::chrono::microseconds>(endLayer - startLayer);
    
    std::cout << "Layer filtering: " << layerDuration.count() << " µs" << std::endl;
    for (size_t i = 0; i < layerCounts.size(); ++i) {
        std::cout << "  Layer " << i << ": " << layerCounts[i] << " colliders" << std::endl;
    }
    std::cout << std::endl;

    // 内存使用统计
    std::cout << "--- Memory Usage ---" << std::endl;
    std::cout << "Total collision data: " << (storage->getMemoryUsage() / 1024) << " KB" << std::endl;
    std::cout << "  BoundingBox mins: " << (mins.capacity() * sizeof(glm::vec3) / 1024) << " KB" << std::endl;
    std::cout << "  BoundingBox maxs: " << (maxs.capacity() * sizeof(glm::vec3) / 1024) << " KB" << std::endl;
    std::cout << "  Collision layers: " << (layers.capacity() * sizeof(uint32_t) / 1024) << " KB" << std::endl;
    std::cout << std::endl;

    std::cout << "=== Summary ===" << std::endl;
    std::cout << "✓ SOA layout: Perfect for collision detection!" << std::endl;
    std::cout << "✓ Cache efficiency: All AABB data contiguous in memory" << std::endl;
    std::cout << "✓ Layer filtering: Fast batch processing by collision layer" << std::endl;
    std::cout << "✓ Real-world benefit: Physics engines love SOA!" << std::endl;
    std::cout << std::endl;

    world->shutdown();
}

int main() {
    std::cout << "Frostbite + SOA Hybrid Architecture Demo" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Combining OOP flexibility with SOA performance!" << std::endl;

    demonstrateSOABatchProcessing();
    demonstrateCollisionBatchProcessing();

    std::cout << "\nDemo complete!" << std::endl;
    return 0;
}
