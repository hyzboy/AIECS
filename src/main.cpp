#include <iostream>
#include <chrono>
#include <glm/glm.hpp>
#include "World.h"
#include "GameEntity.h"
#include "TransformComponentFB.h"
#include "TransformDataStorage.h"

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
        auto transform = entity->addComponent<TransformComponentFB>();
        
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
        auto transform = entity->getComponent<TransformComponentFB>();
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
    
    auto storage = TransformComponentFB::getSharedStorage();
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

int main() {
    std::cout << "Frostbite + SOA Hybrid Architecture Demo" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Combining OOP flexibility with SOA performance!" << std::endl;

    demonstrateSOABatchProcessing();

    std::cout << "\nDemo complete!" << std::endl;
    return 0;
}
