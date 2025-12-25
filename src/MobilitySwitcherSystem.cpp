#include "MobilitySwitcherSystem.h"
#include "MobilitySwitcherComponent.h"
#include "TransformComponent.h"
#include "GameEntity.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/constants.hpp>
#include <iostream>

MobilitySwitcherSystem::MobilitySwitcherSystem(const std::string& name)
    : Module(name)
    , rng(std::random_device{}())  // Use random seed
    , rotSpeedDist(-2.0f, 2.0f)
    , velocityDist(-0.1f, 0.1f) {
}

MobilitySwitcherSystem::~MobilitySwitcherSystem() {
    shutdown();
}

void MobilitySwitcherSystem::initialize() {
    std::cout << "[MobilitySwitcherSystem] Initializing mobility switcher system..." << std::endl;
    initialized = true;
}

void MobilitySwitcherSystem::update(float deltaTime) {
    if (!initialized) return;

    auto worldPtr = world.lock();
    if (!worldPtr) return;

    currentTime += deltaTime;

    // Iterate through all entities in the world
    const auto& objects = worldPtr->getObjects();
    for (const auto& obj : objects) {
        // Try to cast to GameEntity
        auto entity = std::dynamic_pointer_cast<GameEntity>(obj);
        if (!entity) continue;

        // Check if entity has MobilitySwitcherComponent
        auto switcher = entity->getComponent<MobilitySwitcherComponent>();
        if (!switcher) continue;

        // Also need TransformComponent to actually switch mobility
        auto transform = entity->getComponent<TransformComponent>();
        if (!transform) continue;

        // Process mobility switching logic
        if (switcher->isCurrentlyMoving()) {
            // Currently in movable state, check if movement duration is over
            if (currentTime >= switcher->getMovementEndTime()) {
                // Switch back to Static
                transform->setMobility(TransformMobility::Static);
                switcher->setCurrentlyMoving(false);
                
                // Schedule next switch with random interval
                std::uniform_real_distribution<float> intervalDist(
                    switcher->getMinInterval(), 
                    switcher->getMaxInterval()
                );
                switcher->setNextSwitchTime(currentTime + intervalDist(rng));
            } else {
                // Continue moving - apply movement velocity
                glm::vec3 currentPos = transform->getLocalPosition();
                glm::vec3 newPos = currentPos + switcher->getMovementVelocity() * deltaTime;
                
                // Keep within screen bounds
                newPos.x = glm::clamp(newPos.x, -screenBoundary, screenBoundary);
                newPos.y = glm::clamp(newPos.y, -screenBoundary, screenBoundary);
                
                transform->setLocalPosition(newPos);
                
                // Apply rotation (rotation is cumulative)
                glm::quat currentRot = transform->getLocalRotation();
                glm::quat deltaRot = glm::angleAxis(
                    switcher->getRotationSpeed() * deltaTime,
                    glm::vec3(0.0f, 0.0f, 1.0f)
                );
                transform->setLocalRotation(deltaRot * currentRot);
            }
        } else {
            // Currently in static state, check if it's time to switch
            if (currentTime >= switcher->getNextSwitchTime()) {
                // Switch to Movable
                transform->setMobility(TransformMobility::Movable);
                switcher->setCurrentlyMoving(true);
                switcher->setMovementEndTime(currentTime + switcher->getMovementDuration());
                
                // Generate new random movement parameters for variety
                float newRotSpeed = rotSpeedDist(rng);
                glm::vec3 newVelocity(velocityDist(rng), velocityDist(rng), 0.0f);
                switcher->setMovementParameters(newRotSpeed, newVelocity);
            }
        }
    }
}

void MobilitySwitcherSystem::shutdown() {
    std::cout << "[MobilitySwitcherSystem] Shutting down..." << std::endl;
    initialized = false;
}
