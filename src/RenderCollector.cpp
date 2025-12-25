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
    std::cout << "[RenderCollector] Initializing with mobility-based separation..." << std::endl;
    
    // Pre-allocate buffers for performance
    staticModelMatrices.reserve(100);
    staticMaterialIDs.reserve(100);
    movableModelMatrices.reserve(100);
    movableMaterialIDs.reserve(100);
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

    // Clear movable data every frame
    movableModelMatrices.clear();
    movableMaterialIDs.clear();
    
    // Only rebuild static data if dirty
    if (staticDataDirty) {
        staticModelMatrices.clear();
        staticMaterialIDs.clear();
        staticDataDirty = false;
    }
    
    // Clear material deduplication data each frame
    uniqueMaterials.clear();
    materialToID.clear();

    // Collect all entities with both RenderComponent and TransformComponent
    const auto& objects = worldPtr->getObjects();
    
    unsigned int staticCount = 0;
    unsigned int movableCount = 0;
    
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

            // Get transform matrix
            glm::mat4 worldMatrix = transformComp->getWorldMatrix();
            
            // Separate by mobility type
            TransformMobility mobility = transformComp->getMobility();
            if (mobility == TransformMobility::Static || mobility == TransformMobility::Stationary) {
                // Static/Stationary objects - only add if rebuilding
                if (staticModelMatrices.size() <= staticCount) {
                    staticModelMatrices.push_back(worldMatrix);
                    staticMaterialIDs.push_back(matID);
                    staticCount++;
                }
            } else {
                // Movable objects - always update
                movableModelMatrices.push_back(worldMatrix);
                movableMaterialIDs.push_back(matID);
                movableCount++;
            }
        }
    }

    // Extract colors from deduplicated materials for rendering
    std::vector<glm::vec4> materialColors;
    materialColors.reserve(uniqueMaterials.size());
    for (const auto& mat : uniqueMaterials) {
        materialColors.push_back(mat->getColor());
    }

    // Combine static and movable data for rendering
    std::vector<glm::mat4> allMatrices;
    std::vector<unsigned int> allMaterialIDs;
    
    allMatrices.reserve(staticModelMatrices.size() + movableModelMatrices.size());
    allMaterialIDs.reserve(staticMaterialIDs.size() + movableMaterialIDs.size());
    
    // Add static objects first
    allMatrices.insert(allMatrices.end(), staticModelMatrices.begin(), staticModelMatrices.end());
    allMaterialIDs.insert(allMaterialIDs.end(), staticMaterialIDs.begin(), staticMaterialIDs.end());
    
    // Add movable objects
    allMatrices.insert(allMatrices.end(), movableModelMatrices.begin(), movableModelMatrices.end());
    allMaterialIDs.insert(allMaterialIDs.end(), movableMaterialIDs.begin(), movableMaterialIDs.end());

    // Batch render all collected data with deduplicated materials
    if (!allMatrices.empty()) {
        renderSystemPtr->renderBatch(allMatrices, materialColors, allMaterialIDs);
        
        // Log separation stats
        std::cout << "[RenderCollector] Rendered " << allMatrices.size() << " instances: "
                  << staticModelMatrices.size() << " static/stationary, "
                  << movableModelMatrices.size() << " movable | "
                  << uniqueMaterials.size() << " unique materials";
        
        if (uniqueMaterials.size() < allMatrices.size()) {
            std::cout << " (saved " << (allMatrices.size() - uniqueMaterials.size()) 
                      << " material uploads)";
        }
        std::cout << std::endl;
    }
}
