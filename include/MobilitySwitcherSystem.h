#pragma once

#include "Module.h"
#include "World.h"
#include <random>
#include <memory>

/// System that manages mobility switching for entities with MobilitySwitcherComponent
/// Following ECS pattern: System iterates over entities with specific components
/// and processes their behavior each frame
class MobilitySwitcherSystem : public Module {
public:
    MobilitySwitcherSystem(const std::string& name = "MobilitySwitcherSystem");
    ~MobilitySwitcherSystem() override;

    void initialize() override;
    void update(float deltaTime) override;
    void shutdown() override;

    /// Set the world reference for entity iteration
    void setWorld(std::shared_ptr<World> world) { this->world = world; }

    /// Set screen boundaries for position clamping
    void setScreenBoundary(float boundary) { screenBoundary = boundary; }
    float getScreenBoundary() const { return screenBoundary; }

    /// Get random number generator for external use (e.g., initialization)
    std::mt19937& getRNG() { return rng; }

private:
    std::weak_ptr<World> world;
    std::mt19937 rng;  // Random number generator
    
    // Random distributions
    std::uniform_real_distribution<float> rotSpeedDist;
    std::uniform_real_distribution<float> velocityDist;
    
    float currentTime = 0.0f;
    float screenBoundary = 0.95f;
};
