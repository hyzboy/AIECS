#pragma once

#include "Module.h"
#include "RenderSystem.h"
#include "Material.h"
#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>
#include <memory>

class World;
class GameEntity;

/// Collector module that gathers RenderComponent data from all entities
/// and prepares it for batch rendering with material deduplication
class RenderCollector : public Module {
public:
    RenderCollector(const std::string& name = "RenderCollector");
    ~RenderCollector() override = default;

    void initialize() override;
    void update(float deltaTime) override;
    void shutdown() override;

    /// Set the world to collect from
    void setWorld(std::shared_ptr<World> world) { this->world = world; }

    /// Set the render system to use for drawing
    void setRenderSystem(std::shared_ptr<RenderSystem> renderSys) { this->renderSystem = renderSys; }

    /// Collect all RenderComponent data and render
    void collectAndRender();
    
    /// Mark data as needing rebuild (call when entities are added/removed)
    void markDataDirty() { dataInitialized = false; }

private:
    std::weak_ptr<World> world;
    std::weak_ptr<RenderSystem> renderSystem;

    // Temporary buffers for batch data - separated by mobility
    // Static/Stationary objects (rarely updated) - built once and cached
    std::vector<glm::mat4> staticModelMatrices;
    std::vector<unsigned int> staticMaterialIDs;
    
    // Movable objects (updated every frame) - only matrices change
    std::vector<glm::mat4> movableModelMatrices;
    std::vector<unsigned int> movableMaterialIDs;  // Fixed, never changes
    
    // Deduplicated materials - separated by mutability
    std::vector<MaterialPtr> uniqueStaticMaterials;  // Built once, never changes
    std::vector<MaterialPtr> uniqueDynamicMaterials;  // Built once, never changes
    std::unordered_map<MaterialPtr, unsigned int> materialToID;  // Built once, never changes
    
    // Track which entities belong to which category (built once)
    std::vector<std::shared_ptr<GameEntity>> staticEntities;
    std::vector<std::shared_ptr<GameEntity>> movableEntities;
    
    // Track if static data needs to be rebuilt
    bool dataInitialized = false;
};
