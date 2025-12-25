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
    std::cout << "[RenderCollector] Initializing with mobility and material mutability separation..." << std::endl;
    
    // Pre-allocate buffers for performance
    staticModelMatrices.reserve(100);
    staticMaterialIDs.reserve(100);
    movableModelMatrices.reserve(100);
    movableMaterialIDs.reserve(100);
    uniqueStaticMaterials.reserve(20);
    uniqueDynamicMaterials.reserve(20);
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
    
    // Clear dynamic materials every frame
    uniqueDynamicMaterials.clear();
    
    // Only rebuild static data if dirty
    if (staticDataDirty) {
        staticModelMatrices.clear();
        staticMaterialIDs.clear();
        staticDataDirty = false;
    }
    
    // Only rebuild static materials if dirty
    if (staticMaterialsDirty) {
        uniqueStaticMaterials.clear();
        staticMaterialsDirty = false;
    }
    
    // Clear material deduplication mapping each frame (rebuild from scratch)
    materialToID.clear();
    
    // Build material ID mapping starting with static materials
    for (const auto& mat : uniqueStaticMaterials) {
        materialToID[mat] = static_cast<unsigned int>(materialToID.size());
    }

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

            // Deduplicate materials - separate static and dynamic
            unsigned int matID;
            auto it = materialToID.find(material);
            if (it != materialToID.end()) {
                // Material already exists, reuse ID
                matID = it->second;
            } else {
                // New material, assign new ID and add to appropriate list
                matID = static_cast<unsigned int>(materialToID.size());
                materialToID[material] = matID;
                
                if (material->isStatic()) {
                    uniqueStaticMaterials.push_back(material);
                } else {
                    uniqueDynamicMaterials.push_back(material);
                }
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
    // Static materials
    std::vector<glm::vec4> staticMaterialColors;
    staticMaterialColors.reserve(uniqueStaticMaterials.size());
    for (const auto& mat : uniqueStaticMaterials) {
        staticMaterialColors.push_back(mat->getColor());
    }
    
    // Dynamic materials
    std::vector<glm::vec4> dynamicMaterialColors;
    dynamicMaterialColors.reserve(uniqueDynamicMaterials.size());
    for (const auto& mat : uniqueDynamicMaterials) {
        dynamicMaterialColors.push_back(mat->getColor());
    }

    // Batch render with dual SSBO/VBO architecture
    if (!staticModelMatrices.empty() || !movableModelMatrices.empty()) {
        renderSystemPtr->renderBatch(staticModelMatrices, staticMaterialColors, staticMaterialIDs,
                                      movableModelMatrices, dynamicMaterialColors, movableMaterialIDs);
        
        // Log separation stats
        size_t totalInstances = staticModelMatrices.size() + movableModelMatrices.size();
        size_t totalUniqueMaterials = uniqueStaticMaterials.size() + uniqueDynamicMaterials.size();
        
        std::cout << "[RenderCollector] Rendered " << totalInstances << " instances: "
                  << staticModelMatrices.size() << " static/stationary (GL_STATIC_DRAW), "
                  << movableModelMatrices.size() << " movable (GL_DYNAMIC_DRAW) | "
                  << uniqueStaticMaterials.size() << " static materials (GL_STATIC_DRAW), "
                  << uniqueDynamicMaterials.size() << " dynamic materials (GL_DYNAMIC_DRAW)";
        
        if (totalUniqueMaterials < totalInstances) {
            std::cout << " (saved " << (totalInstances - totalUniqueMaterials) 
                      << " material uploads)";
        }
        std::cout << std::endl;
    }
}
