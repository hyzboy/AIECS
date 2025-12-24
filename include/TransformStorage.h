#pragma once

#include <vector>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
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
            parents[id] = INVALID_ENTITY;
            children[id].clear();
            worldMatrices[id] = glm::mat4(1.0f);
        } else {
            // Allocate a new slot
            id = static_cast<EntityID>(positions.size());
            positions.emplace_back(0.0f);
            rotations.emplace_back(1.0f, 0.0f, 0.0f, 0.0f);
            scales.emplace_back(1.0f);
            parents.emplace_back(INVALID_ENTITY);
            children.emplace_back();
            worldMatrices.emplace_back(1.0f);
        }
        return id;
    }

    /// Free a transform slot for reuse
    void deallocate(EntityID id) {
        if (id < positions.size()) {
            // Remove this entity from its parent's children list
            if (parents[id] != INVALID_ENTITY) {
                removeChild(parents[id], id);
            }
            
            // Remove all children relationships (but don't deallocate them)
            for (EntityID childId : children[id]) {
                if (childId < parents.size()) {
                    parents[childId] = INVALID_ENTITY;
                }
            }
            
            freeList.push_back(id);
        }
    }

    /// Check if an entity ID is valid (allocated and not freed)
    bool isValid(EntityID id) const {
        if (id >= positions.size()) {
            return false;
        }
        // Check if the ID is in the free list
        for (const auto& freeId : freeList) {
            if (freeId == id) {
                return false;
            }
        }
        return true;
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

    // Parent-child relationship accessors
    EntityID getParent(EntityID id) const {
        if (id >= parents.size()) return INVALID_ENTITY;
        return parents[id];
    }

    void setParent(EntityID id, EntityID parentId) {
        if (id >= parents.size()) return;
        
        // Remove from old parent's children list
        if (parents[id] != INVALID_ENTITY) {
            removeChild(parents[id], id);
        }
        
        // Set new parent
        parents[id] = parentId;
        
        // Add to new parent's children list
        if (parentId != INVALID_ENTITY && parentId < children.size()) {
            children[parentId].push_back(id);
        }
    }

    const std::vector<EntityID>& getChildren(EntityID id) const {
        static const std::vector<EntityID> emptyList;
        if (id >= children.size()) return emptyList;
        return children[id];
    }

    void addChild(EntityID parentId, EntityID childId) {
        if (parentId >= children.size() || childId >= parents.size()) return;
        
        // Remove child from its old parent
        if (parents[childId] != INVALID_ENTITY) {
            removeChild(parents[childId], childId);
        }
        
        // Set new parent-child relationship
        parents[childId] = parentId;
        children[parentId].push_back(childId);
    }

    void removeChild(EntityID parentId, EntityID childId) {
        if (parentId >= children.size()) return;
        
        auto& childList = children[parentId];
        auto it = std::find(childList.begin(), childList.end(), childId);
        if (it != childList.end()) {
            childList.erase(it);
        }
    }

    // World transform matrix accessors
    const glm::mat4& getWorldMatrix(EntityID id) const {
        static const glm::mat4 identity(1.0f);
        if (!isValid(id)) return identity;
        return worldMatrices[id];
    }

    void setWorldMatrix(EntityID id, const glm::mat4& matrix) {
        if (isValid(id)) {
            worldMatrices[id] = matrix;
        }
    }

    /// Compute local transform matrix from position, rotation, and scale
    glm::mat4 computeLocalMatrix(EntityID id) const {
        if (!isValid(id)) return glm::mat4(1.0f);
        
        // Create transformation matrix: T * R * S
        glm::mat4 matrix(1.0f);
        
        // Apply translation
        matrix = glm::translate(matrix, positions[id]);
        
        // Apply rotation
        matrix = matrix * glm::mat4_cast(rotations[id]);
        
        // Apply scale
        matrix = glm::scale(matrix, scales[id]);
        
        return matrix;
    }

    /// Update world transform matrix for an entity
    /// If it has a parent, multiplies parent's world matrix with local matrix
    /// Otherwise, world matrix = local matrix
    void updateWorldMatrix(EntityID id) {
        if (!isValid(id)) return;
        
        glm::mat4 localMatrix = computeLocalMatrix(id);
        
        if (parents[id] != INVALID_ENTITY && isValid(parents[id])) {
            // Has parent: world = parent_world * local
            worldMatrices[id] = worldMatrices[parents[id]] * localMatrix;
        } else {
            // No parent: world = local
            worldMatrices[id] = localMatrix;
        }
    }

    /// Update world transform matrix hierarchically for an entity and all its children
    void updateWorldMatrixHierarchy(EntityID id) {
        if (!isValid(id)) return;
        
        // Update this entity's world matrix
        updateWorldMatrix(id);
        
        // Recursively update all children
        if (id < children.size()) {
            for (EntityID childId : children[id]) {
                updateWorldMatrixHierarchy(childId);
            }
        }
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
    
    // World transform matrices (computed from hierarchy)
    std::vector<glm::mat4> worldMatrices;
    
    // Parent-child relationships (SOA style)
    std::vector<EntityID> parents;
    std::vector<std::vector<EntityID>> children;
    
    // Free list for reusing deallocated slots
    std::vector<EntityID> freeList;
};
