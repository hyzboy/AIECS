#include "RenderCollector.h"
#include "World.h"
#include "GameEntity.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include <iostream>

RenderCollector::RenderCollector(const std::string& name)
    : EntitySystem(name) {
}

void RenderCollector::initialize() {
    std::cout << "[RenderCollector] Initializing with zero-touch static optimization..." << std::endl;
    
    // Pre-allocate buffers for performance
    staticModelMatrices.reserve(100);
    staticMaterialIDs.reserve(100);
    movableModelMatrices.reserve(100);
    movableMaterialIDs.reserve(100);
    uniqueStaticMaterials.reserve(20);
    uniqueDynamicMaterials.reserve(20);
    staticEntities.reserve(100);
    movableEntities.reserve(100);
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

    // Initialize data structure on first frame only
    if (!dataInitialized) {
        std::cout << "[RenderCollector] First frame - building static data structures..." << std::endl;
        
        // Clear all data
        staticModelMatrices.clear();
        staticMaterialIDs.clear();
        movableModelMatrices.clear();
        movableMaterialIDs.clear();
        uniqueStaticMaterials.clear();
        uniqueDynamicMaterials.clear();
        materialToID.clear();
        staticEntities.clear();
        movableEntities.clear();
        
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

                // Deduplicate materials (do once)
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
                if (mobility == TransformMobility::Static) {
                    // Static objects - store entity reference and data
                    staticEntities.push_back(entity);
                    staticModelMatrices.push_back(worldMatrix);
                    staticMaterialIDs.push_back(matID);
                } else {
                    // Movable objects - store entity reference and data
                    movableEntities.push_back(entity);
                    movableModelMatrices.push_back(worldMatrix);
                    movableMaterialIDs.push_back(matID);  // Material ID never changes!
                }
            }
        }
        
        dataInitialized = true;
        std::cout << "[RenderCollector] Static data initialized: "
                  << staticEntities.size() << " static, "
                  << movableEntities.size() << " movable | "
                  << uniqueStaticMaterials.size() << " static materials, "
                  << uniqueDynamicMaterials.size() << " dynamic materials" << std::endl;
    } else {
        // After first frame: Only update movable matrices (not VBO IDs, not materials!)
        movableModelMatrices.clear();
        for (size_t i = 0; i < movableEntities.size(); ++i) {
            auto entity = movableEntities[i];
            auto transformComp = entity->getComponent<TransformComponent>();
            if (transformComp) {
                movableModelMatrices.push_back(transformComp->getWorldMatrix());
            }
        }
        // Note: movableMaterialIDs never changes after initialization!
    }

    // Extract colors from deduplicated materials for rendering (do once on init)
    // Static materials (never changes after initialization)
    std::vector<glm::vec4> staticMaterialColors;
    staticMaterialColors.reserve(uniqueStaticMaterials.size());
    for (const auto& mat : uniqueStaticMaterials) {
        staticMaterialColors.push_back(mat->getColor());
    }
    
    // Dynamic materials (never changes after initialization)
    std::vector<glm::vec4> dynamicMaterialColors;
    dynamicMaterialColors.reserve(uniqueDynamicMaterials.size());
    for (const auto& mat : uniqueDynamicMaterials) {
        dynamicMaterialColors.push_back(mat->getColor());
    }

    // Batch render with dual SSBO/VBO architecture
    if (!staticModelMatrices.empty() || !movableModelMatrices.empty()) {
        renderSystemPtr->renderBatch(staticModelMatrices, staticMaterialColors, staticMaterialIDs,
                                      movableModelMatrices, dynamicMaterialColors, movableMaterialIDs);
    }
}
