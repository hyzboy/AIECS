#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <cstdint>
#include <optional>

/// SOA (Structure of Arrays) storage for Transform components
/// Stores position, rotation, and scale separately for better cache performance
class TransformStorage {
public:
    using EntityID = uint32_t;
    static constexpr EntityID INVALID_ENTITY = UINT32_MAX;

    TransformStorage() = default;

    /// Allocate a new transform slot and return its index
    EntityID allocate() {
        EntityID id;
        if (!freeList.empty()) {
            // Reuse a freed slot
            id = freeList.back();
            freeList.pop_back();
            
            // Reset the transform data
            positions[id] = glm::vec3(0.0f);
            rotations[id] = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
            scales[id] = glm::vec3(1.0f);
        } else {
            // Allocate a new slot
            id = static_cast<EntityID>(positions.size());
            positions.emplace_back(0.0f);
            rotations.emplace_back(1.0f, 0.0f, 0.0f, 0.0f);
            scales.emplace_back(1.0f);
        }
        return id;
    }

    /// Free a transform slot for reuse
    void deallocate(EntityID id) {
        if (id < positions.size()) {
            freeList.push_back(id);
        }
    }

    /// Check if an entity ID is valid
    bool isValid(EntityID id) const {
        return id < positions.size();
    }

    // Position accessors
    const glm::vec3& getPosition(EntityID id) const {
        return positions[id];
    }

    void setPosition(EntityID id, const glm::vec3& pos) {
        positions[id] = pos;
    }

    // Rotation accessors
    const glm::quat& getRotation(EntityID id) const {
        return rotations[id];
    }

    void setRotation(EntityID id, const glm::quat& rot) {
        rotations[id] = rot;
    }

    // Scale accessors
    const glm::vec3& getScale(EntityID id) const {
        return scales[id];
    }

    void setScale(EntityID id, const glm::vec3& scale) {
        scales[id] = scale;
    }

    /// Get the number of allocated transform slots
    size_t size() const {
        return positions.size();
    }

private:
    // SOA storage: separate arrays for better cache locality
    std::vector<glm::vec3> positions;
    std::vector<glm::quat> rotations;
    std::vector<glm::vec3> scales;
    
    // Free list for reusing deallocated slots
    std::vector<EntityID> freeList;
};
