#pragma once

#include "ComponentTypes.h"
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>

/// SOA storage for Collision components
class CollisionStorage {
public:
    CollisionStorage() = default;

    /// Allocate a new collision component slot
    ComponentID allocate() {
        ComponentID compId;
        if (!freeList.empty()) {
            compId = freeList.back();
            freeList.pop_back();
            
            // Reset data
            boundingBoxMin[compId] = glm::vec3(-0.5f);
            boundingBoxMax[compId] = glm::vec3(0.5f);
            collisionLayer[compId] = 0;
            isEnabled[compId] = true;
        } else {
            compId = static_cast<ComponentID>(boundingBoxMin.size());
            boundingBoxMin.emplace_back(-0.5f);
            boundingBoxMax.emplace_back(0.5f);
            collisionLayer.emplace_back(0);
            isEnabled.emplace_back(true);
        }
        return compId;
    }

    /// Free a collision component slot
    void deallocate(ComponentID compId) {
        if (compId < boundingBoxMin.size()) {
            freeList.push_back(compId);
        }
    }

    /// Check if component ID is valid
    bool isValid(ComponentID compId) const {
        if (compId >= boundingBoxMin.size()) return false;
        return std::find(freeList.begin(), freeList.end(), compId) == freeList.end();
    }

    /// Get bounding box min
    const glm::vec3& getBoundingBoxMin(ComponentID compId) const {
        return boundingBoxMin[compId];
    }

    void setBoundingBoxMin(ComponentID compId, const glm::vec3& min) {
        boundingBoxMin[compId] = min;
    }

    /// Get bounding box max
    const glm::vec3& getBoundingBoxMax(ComponentID compId) const {
        return boundingBoxMax[compId];
    }

    void setBoundingBoxMax(ComponentID compId, const glm::vec3& max) {
        boundingBoxMax[compId] = max;
    }

    /// Get collision layer
    uint32_t getCollisionLayer(ComponentID compId) const {
        return collisionLayer[compId];
    }

    void setCollisionLayer(ComponentID compId, uint32_t layer) {
        collisionLayer[compId] = layer;
    }

    /// Check if collision is enabled
    bool getEnabled(ComponentID compId) const {
        return isEnabled[compId];
    }

    void setEnabled(ComponentID compId, bool enabled) {
        isEnabled[compId] = enabled;
    }

    size_t size() const {
        return boundingBoxMin.size();
    }

private:
    std::vector<glm::vec3> boundingBoxMin;
    std::vector<glm::vec3> boundingBoxMax;
    std::vector<uint32_t> collisionLayer;
    std::vector<bool> isEnabled;
    std::vector<ComponentID> freeList;
};
