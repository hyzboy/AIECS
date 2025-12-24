#pragma once

#include "CollisionStorage.h"
#include "ComponentTypes.h"
#include <glm/glm.hpp>

/// CollisionComponent accessor for reading and writing collision data
class CollisionComponent {
public:
    CollisionComponent(ComponentID compId, CollisionStorage* storage)
        : componentId(compId), collisionStorage(storage) {}

    /// Check if this component is valid
    bool isValid() const {
        return componentId != INVALID_COMPONENT && 
               collisionStorage != nullptr && 
               collisionStorage->isValid(componentId);
    }

    /// Get component ID
    ComponentID getComponentID() const {
        return componentId;
    }

    /// Get bounding box min
    glm::vec3 getBoundingBoxMin() const {
        if (!isValid()) return glm::vec3(-0.5f);
        return collisionStorage->getBoundingBoxMin(componentId);
    }

    void setBoundingBoxMin(const glm::vec3& min) {
        if (isValid()) {
            collisionStorage->setBoundingBoxMin(componentId, min);
        }
    }

    /// Get bounding box max
    glm::vec3 getBoundingBoxMax() const {
        if (!isValid()) return glm::vec3(0.5f);
        return collisionStorage->getBoundingBoxMax(componentId);
    }

    void setBoundingBoxMax(const glm::vec3& max) {
        if (isValid()) {
            collisionStorage->setBoundingBoxMax(componentId, max);
        }
    }

    /// Set bounding box from min and max
    void setBoundingBox(const glm::vec3& min, const glm::vec3& max) {
        if (isValid()) {
            collisionStorage->setBoundingBoxMin(componentId, min);
            collisionStorage->setBoundingBoxMax(componentId, max);
        }
    }

    /// Get collision layer
    uint32_t getCollisionLayer() const {
        if (!isValid()) return 0;
        return collisionStorage->getCollisionLayer(componentId);
    }

    void setCollisionLayer(uint32_t layer) {
        if (isValid()) {
            collisionStorage->setCollisionLayer(componentId, layer);
        }
    }

    /// Check if collision is enabled
    bool isEnabled() const {
        if (!isValid()) return false;
        return collisionStorage->getEnabled(componentId);
    }

    void setEnabled(bool enabled) {
        if (isValid()) {
            collisionStorage->setEnabled(componentId, enabled);
        }
    }

private:
    ComponentID componentId;
    CollisionStorage* collisionStorage;
};
