#pragma once

#include "GameEntity.h"
#include <glm/glm.hpp>

/// EntityComponent that enables automatic mobility switching for static/movable transitions
/// Entities with this component will periodically switch from Static to Movable,
/// move for a specified duration, then switch back to Static
class MobilitySwitcherComponent : public EntityComponent {
public:
    MobilitySwitcherComponent(const std::string& name = "MobilitySwitcher");
    ~MobilitySwitcherComponent() override = default;

    /// Configure switching behavior
    /// @param minInterval - Minimum time between switches in seconds
    /// @param maxInterval - Maximum time between switches in seconds
    /// @param duration - How long to stay in Movable state in seconds
    void configure(float minInterval, float maxInterval, float duration);

    /// Set random movement parameters (rotation speed and velocity)
    void setMovementParameters(float rotationSpeed, const glm::vec3& velocity);

    /// Get next switch time
    float getNextSwitchTime() const { return nextSwitchTime; }
    void setNextSwitchTime(float time) { nextSwitchTime = time; }

    /// Get movement end time
    float getMovementEndTime() const { return movementEndTime; }
    void setMovementEndTime(float time) { movementEndTime = time; }

    /// Check if currently moving
    bool isCurrentlyMoving() const { return currentlyMoving; }
    void setCurrentlyMoving(bool moving) { currentlyMoving = moving; }

    /// Get movement parameters
    float getRotationSpeed() const { return rotationSpeed; }
    const glm::vec3& getMovementVelocity() const { return movementVelocity; }

    /// Get interval configuration
    float getMinInterval() const { return minInterval; }
    float getMaxInterval() const { return maxInterval; }
    float getMovementDuration() const { return movementDuration; }

    void onAttach() override;
    void onUpdate(float deltaTime) override;

private:
    // Timing state
    float nextSwitchTime = 0.0f;      // When to next switch to Movable
    float movementEndTime = 0.0f;     // When to switch back to Static
    bool currentlyMoving = false;     // Current state

    // Movement parameters (regenerated each cycle)
    float rotationSpeed = 0.0f;
    glm::vec3 movementVelocity = glm::vec3(0.0f);

    // Configuration
    float minInterval = 2.0f;         // Minimum time between switches
    float maxInterval = 5.0f;         // Maximum time between switches
    float movementDuration = 1.0f;    // How long to move when active
};
