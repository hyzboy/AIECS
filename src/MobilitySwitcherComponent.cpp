#include "MobilitySwitcherComponent.h"
#include <iostream>

MobilitySwitcherComponent::MobilitySwitcherComponent(const std::string& name)
    : EntityComponent(name) {
}

void MobilitySwitcherComponent::configure(float minInterval, float maxInterval, float duration) {
    this->minInterval = minInterval;
    this->maxInterval = maxInterval;
    this->movementDuration = duration;
}

void MobilitySwitcherComponent::setMovementParameters(float rotationSpeed, const glm::vec3& velocity) {
    this->rotationSpeed = rotationSpeed;
    this->movementVelocity = velocity;
}

void MobilitySwitcherComponent::onAttach() {
    // EntityComponent attached - initialization handled by System
}

void MobilitySwitcherComponent::onUpdate(float deltaTime) {
    // Update handled by MobilitySwitcherSystem
    // Keeping this empty follows ECS pattern where Systems process Components
}
