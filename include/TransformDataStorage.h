#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <unordered_map>
#include <functional>
#include <cstdint>

/// Optimized SOA storage for transform data
/// Used by TransformComponentFB for better cache performance
class TransformDataStorage {
public:
    using HandleID = uint32_t;
    static constexpr HandleID INVALID_HANDLE = UINT32_MAX;

    /// Allocate space for a new transform
    HandleID allocate() {
        HandleID id = static_cast<HandleID>(positions.size());
        positions.emplace_back(0.0f);
        rotations.emplace_back(1.0f, 0.0f, 0.0f, 0.0f);
        scales.emplace_back(1.0f);
        worldMatrices.emplace_back(1.0f);
        parentHandles.push_back(INVALID_HANDLE);
        matrixDirty.push_back(true);
        mobility.push_back(1);  // Default to Movable
        return id;
    }

    /// Free allocated space
    void deallocate(HandleID id) {
        if (id >= positions.size()) return;
        
        // Mark as unused but keep data (can be reused)
        // For true removal, use compaction (defragmentation)
    }

    // Position accessors - SOA optimized
    glm::vec3 getPosition(HandleID id) const {
        return positions[id];
    }

    void setPosition(HandleID id, const glm::vec3& pos) {
        positions[id] = pos;
        matrixDirty[id] = true;
    }

    // Rotation accessors - SOA optimized
    glm::quat getRotation(HandleID id) const {
        return rotations[id];
    }

    void setRotation(HandleID id, const glm::quat& rot) {
        rotations[id] = rot;
        matrixDirty[id] = true;
    }

    // Scale accessors - SOA optimized
    glm::vec3 getScale(HandleID id) const {
        return scales[id];
    }

    void setScale(HandleID id, const glm::vec3& scale) {
        scales[id] = scale;
        matrixDirty[id] = true;
    }

    // Matrix accessors
    glm::mat4 getWorldMatrix(HandleID id) const {
        return worldMatrices[id];
    }

    void setWorldMatrix(HandleID id, const glm::mat4& matrix) {
        worldMatrices[id] = matrix;
        matrixDirty[id] = false;
    }

    // Parent relationship
    HandleID getParent(HandleID id) const {
        return parentHandles[id];
    }

    void setParent(HandleID id, HandleID parentId) {
        parentHandles[id] = parentId;
        matrixDirty[id] = true;
    }

    // Dirty flag
    bool isDirty(HandleID id) const {
        return matrixDirty[id];
    }

    void setDirty(HandleID id, bool dirty) {
        matrixDirty[id] = dirty;
    }

    // Mobility tracking (0 = Static, 1 = Movable)
    uint8_t getMobility(HandleID id) const {
        return mobility[id];
    }

    void setMobility(HandleID id, uint8_t mobilityValue) {
        mobility[id] = mobilityValue;
    }

    // Batch operations - these are much faster with SOA!

    /// Update only movable dirty transforms - optimized for Static/Movable separation
    /// Static objects are never updated after initialization
    void updateMovableDirtyMatrices(const std::function<void(HandleID, glm::vec3, glm::quat, glm::vec3)>& callback) {
        // Only iterate through movable objects
        for (size_t i = 0; i < matrixDirty.size(); ++i) {
            // Skip static objects (mobility == 0)
            if (mobility[i] == 0) continue;
            
            if (matrixDirty[i]) {
                // Load from cache-friendly consecutive arrays
                callback(static_cast<HandleID>(i), positions[i], rotations[i], scales[i]);
                matrixDirty[i] = false;
            }
        }
    }

    /// Update all dirty transforms - cache friendly
    /// Process all positions, then rotations, then scales
    /// NOTE: Prefer updateMovableDirtyMatrices() for better performance with Static/Movable separation
    void updateAllDirtyMatrices(const std::function<void(HandleID, glm::vec3, glm::quat, glm::vec3)>& callback) {
        // Iterate through all dirty flags
        for (size_t i = 0; i < matrixDirty.size(); ++i) {
            if (matrixDirty[i]) {
                // Load from cache-friendly consecutive arrays
                callback(static_cast<HandleID>(i), positions[i], rotations[i], scales[i]);
                matrixDirty[i] = false;
            }
        }
    }

    /// Get all positions for batch processing
    const std::vector<glm::vec3>& getAllPositions() const { return positions; }
    std::vector<glm::vec3>& getAllPositions() { return positions; }

    /// Get all rotations for batch processing
    const std::vector<glm::quat>& getAllRotations() const { return rotations; }
    std::vector<glm::quat>& getAllRotations() { return rotations; }

    /// Get all scales for batch processing
    const std::vector<glm::vec3>& getAllScales() const { return scales; }
    std::vector<glm::vec3>& getAllScales() { return scales; }

    /// Get all world matrices for batch processing
    const std::vector<glm::mat4>& getAllWorldMatrices() const { return worldMatrices; }
    std::vector<glm::mat4>& getAllWorldMatrices() { return worldMatrices; }

    size_t size() const { return positions.size(); }

    /// Clear all data
    void clear() {
        positions.clear();
        rotations.clear();
        scales.clear();
        worldMatrices.clear();
        parentHandles.clear();
        matrixDirty.clear();
        mobility.clear();
    }

private:
    // SOA - Separate Arrays for each component
    // This layout is much more cache-friendly for batch operations
    std::vector<glm::vec3> positions;        // 12 bytes each, consecutive
    std::vector<glm::quat> rotations;        // 16 bytes each, consecutive
    std::vector<glm::vec3> scales;           // 12 bytes each, consecutive
    std::vector<glm::mat4> worldMatrices;    // 64 bytes each, consecutive
    std::vector<HandleID> parentHandles;     // 4 bytes each, consecutive
    std::vector<bool> matrixDirty;           // 1 byte each, consecutive
    std::vector<uint8_t> mobility;           // 1 byte each, consecutive (0=Static, 1=Movable)
};
