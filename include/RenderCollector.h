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

private:
    std::weak_ptr<World> world;
    std::weak_ptr<RenderSystem> renderSystem;

    // Temporary buffers for batch data - separated by mobility
    // Static/Stationary objects (rarely updated)
    std::vector<glm::mat4> staticModelMatrices;
    std::vector<unsigned int> staticMaterialIDs;
    
    // Movable objects (updated every frame)
    std::vector<glm::mat4> movableModelMatrices;
    std::vector<unsigned int> movableMaterialIDs;
    
    // Deduplicated materials
    std::vector<MaterialPtr> uniqueMaterials;
    std::unordered_map<MaterialPtr, unsigned int> materialToID;
    
    // Track if static data needs to be rebuilt
    bool staticDataDirty = true;
};
