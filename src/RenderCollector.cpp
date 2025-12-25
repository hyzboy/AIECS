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
    materialIDs.reserve(100);
    uniqueMaterials.reserve(20); // Typically fewer unique materials
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
    materialIDs.clear();
    uniqueMaterials.clear();
    materialToID.clear();

    // Collect all entities with both RenderComponent and TransformComponent
    const auto& objects = worldPtr->getObjects();
    
    for (const auto& obj : objects) {
        auto entity = std::dynamic_pointer_cast<GameEntity>(obj);
        if (!entity) continue;

        // Check if entity has both required components
        auto renderComp = entity->getComponent<RenderComponent>();
        auto transformComp = entity->getComponent<TransformComponent>();

        if (renderComp && transformComp && renderComp->getVisible()) {
            // Get material from render component
            auto material = renderComp->getMaterial();
            if (!material) continue;

            // Deduplicate materials
            unsigned int matID;
            auto it = materialToID.find(material);
            if (it != materialToID.end()) {
                // Material already exists, reuse ID
                matID = it->second;
            } else {
                // New material, assign new ID
                matID = static_cast<unsigned int>(uniqueMaterials.size());
                uniqueMaterials.push_back(material);
                materialToID[material] = matID;
            }

            // Collect transform matrix and material ID
            modelMatrices.push_back(transformComp->getWorldMatrix());
            materialIDs.push_back(matID);
        }
    }

    // Extract colors from deduplicated materials for rendering
    std::vector<glm::vec4> materialColors;
    materialColors.reserve(uniqueMaterials.size());
    for (const auto& mat : uniqueMaterials) {
        materialColors.push_back(mat->getColor());
    }

    // Batch render all collected data with deduplicated materials
    if (!modelMatrices.empty()) {
        renderSystemPtr->renderBatch(modelMatrices, materialColors, materialIDs);
        
        // Log deduplication stats (can be disabled in production)
        if (uniqueMaterials.size() < modelMatrices.size()) {
            std::cout << "[RenderCollector] Rendered " << modelMatrices.size() 
                      << " instances with " << uniqueMaterials.size() 
                      << " unique materials (saved " 
                      << (modelMatrices.size() - uniqueMaterials.size()) 
                      << " material uploads)" << std::endl;
        }
    }
}
