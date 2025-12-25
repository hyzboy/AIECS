#include "RenderCollector.h"
#include "World.h"
#include "GameEntity.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include <iostream>

RenderCollector::RenderCollector(const std::string& name)
    : Module(name) {
}

void RenderCollector::initialize() {
    std::cout << "[RenderCollector] Initializing..." << std::endl;
    
    // Pre-allocate buffers for performance
    modelMatrices.reserve(100);
    colors.reserve(100);
}

void RenderCollector::update(float deltaTime) {
    collectAndRender();
}

void RenderCollector::shutdown() {
    std::cout << "[RenderCollector] Shutdown complete." << std::endl;
}

void RenderCollector::collectAndRender() {
    auto worldPtr = world.lock();
    auto renderSystemPtr = renderSystem.lock();
    
    if (!worldPtr || !renderSystemPtr) {
        return;
    }

    // Clear previous frame data
    modelMatrices.clear();
    colors.clear();

    // Collect all entities with both RenderComponent and TransformComponent
    const auto& objects = worldPtr->getObjects();
    
    for (const auto& obj : objects) {
        auto entity = std::dynamic_pointer_cast<GameEntity>(obj);
        if (!entity) continue;

        // Check if entity has both required components
        auto renderComp = entity->getComponent<RenderComponent>();
        auto transformComp = entity->getComponent<TransformComponent>();

        if (renderComp && transformComp && renderComp->getVisible()) {
            // Collect transform matrix and color
            modelMatrices.push_back(transformComp->getWorldMatrix());
            colors.push_back(renderComp->getColor());
        }
    }

    // Batch render all collected data
    if (!modelMatrices.empty()) {
        renderSystemPtr->renderBatch(modelMatrices, colors);
    }
}
