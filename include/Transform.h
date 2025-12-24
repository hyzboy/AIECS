#pragma once

#include "TransformStorage.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

/// Transform accessor class for reading and writing entity transforms
/// Provides both local (relative to parent) and world (absolute) transform access
class Transform {
public:
    using EntityID = TransformStorage::EntityID;

    Transform(EntityID entityId, TransformStorage* storage)
        : id(entityId), transformStorage(storage) {}

    /// Check if this transform is valid
    bool isValid() const {
        return transformStorage != nullptr && transformStorage->isValid(id);
    }

    // Local transform accessors (relative to parent)

    /// Get local position (relative to parent)
    glm::vec3 getLocalPosition() const {
        if (!isValid()) return glm::vec3(0.0f);
        return transformStorage->getPosition(id);
    }

    /// Set local position (relative to parent)
    void setLocalPosition(const glm::vec3& pos) {
        if (isValid()) {
            transformStorage->setPosition(id, pos);
        }
    }

    /// Get local rotation (relative to parent)
    glm::quat getLocalRotation() const {
        if (!isValid()) return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        return transformStorage->getRotation(id);
    }

    /// Set local rotation (relative to parent)
    void setLocalRotation(const glm::quat& rot) {
        if (isValid()) {
            transformStorage->setRotation(id, rot);
        }
    }

    /// Get local scale (relative to parent)
    glm::vec3 getLocalScale() const {
        if (!isValid()) return glm::vec3(1.0f);
        return transformStorage->getScale(id);
    }

    /// Set local scale (relative to parent)
    void setLocalScale(const glm::vec3& scale) {
        if (isValid()) {
            transformStorage->setScale(id, scale);
        }
    }

    /// Set all local transform components at once
    void setLocalTRS(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale) {
        if (isValid()) {
            transformStorage->setPosition(id, position);
            transformStorage->setRotation(id, rotation);
            transformStorage->setScale(id, scale);
        }
    }

    // World transform accessors (absolute, in world space)

    /// Get world transform matrix
    glm::mat4 getWorldMatrix() const {
        if (!isValid()) return glm::mat4(1.0f);
        return transformStorage->getWorldMatrix(id);
    }

    /// Get world position (extracted from world matrix)
    glm::vec3 getWorldPosition() const {
        glm::mat4 worldMat = getWorldMatrix();
        return glm::vec3(worldMat[3]);
    }

    /// Get world rotation (extracted from world matrix)
    glm::quat getWorldRotation() const {
        glm::mat4 worldMat = getWorldMatrix();
        // Extract rotation from matrix (assuming no skew/shear)
        glm::mat3 rotMat(worldMat);
        
        // Remove scale
        glm::vec3 scale = getWorldScale();
        rotMat[0] /= scale.x;
        rotMat[1] /= scale.y;
        rotMat[2] /= scale.z;
        
        return glm::quat_cast(rotMat);
    }

    /// Get world scale (extracted from world matrix)
    glm::vec3 getWorldScale() const {
        glm::mat4 worldMat = getWorldMatrix();
        return glm::vec3(
            glm::length(glm::vec3(worldMat[0])),
            glm::length(glm::vec3(worldMat[1])),
            glm::length(glm::vec3(worldMat[2]))
        );
    }

    /// Set world position (back-calculates local position from parent)
    void setWorldPosition(const glm::vec3& worldPos) {
        if (!isValid()) return;

        EntityID parentId = transformStorage->getParent(id);
        if (parentId == TransformStorage::INVALID_ENTITY) {
            // No parent, world position = local position
            transformStorage->setPosition(id, worldPos);
        } else {
            // Has parent, need to convert world to local
            glm::mat4 parentWorldMatrix = transformStorage->getWorldMatrix(parentId);
            glm::mat4 invParentMatrix = glm::inverse(parentWorldMatrix);
            
            // Transform world position to parent's local space
            glm::vec4 localPos4 = invParentMatrix * glm::vec4(worldPos, 1.0f);
            transformStorage->setPosition(id, glm::vec3(localPos4));
        }
    }

    /// Set world rotation (back-calculates local rotation from parent)
    void setWorldRotation(const glm::quat& worldRot) {
        if (!isValid()) return;

        EntityID parentId = transformStorage->getParent(id);
        if (parentId == TransformStorage::INVALID_ENTITY) {
            // No parent, world rotation = local rotation
            transformStorage->setRotation(id, worldRot);
        } else {
            // Has parent, need to convert world to local
            glm::quat parentWorldRot = extractRotationFromMatrix(transformStorage->getWorldMatrix(parentId));
            glm::quat localRot = glm::inverse(parentWorldRot) * worldRot;
            transformStorage->setRotation(id, localRot);
        }
    }

    /// Set world scale (back-calculates local scale from parent)
    void setWorldScale(const glm::vec3& worldScale) {
        if (!isValid()) return;

        EntityID parentId = transformStorage->getParent(id);
        if (parentId == TransformStorage::INVALID_ENTITY) {
            // No parent, world scale = local scale
            transformStorage->setScale(id, worldScale);
        } else {
            // Has parent, need to divide by parent's world scale
            glm::vec3 parentWorldScale = extractScaleFromMatrix(transformStorage->getWorldMatrix(parentId));
            glm::vec3 localScale = worldScale / parentWorldScale;
            transformStorage->setScale(id, localScale);
        }
    }

    /// Set world transform (back-calculates local TRS from parent)
    void setWorldTRS(const glm::vec3& worldPos, const glm::quat& worldRot, const glm::vec3& worldScale) {
        setWorldPosition(worldPos);
        setWorldRotation(worldRot);
        setWorldScale(worldScale);
    }

    /// Set world transform from matrix (back-calculates local TRS)
    void setWorldMatrix(const glm::mat4& worldMatrix) {
        // Extract TRS from matrix
        glm::vec3 worldPos(worldMatrix[3]);
        glm::vec3 worldScale = extractScaleFromMatrix(worldMatrix);
        glm::quat worldRot = extractRotationFromMatrix(worldMatrix);
        
        setWorldTRS(worldPos, worldRot, worldScale);
    }

private:
    /// Helper: Extract scale from matrix
    static glm::vec3 extractScaleFromMatrix(const glm::mat4& matrix) {
        return glm::vec3(
            glm::length(glm::vec3(matrix[0])),
            glm::length(glm::vec3(matrix[1])),
            glm::length(glm::vec3(matrix[2]))
        );
    }

    /// Helper: Extract rotation from matrix (removes scale)
    static glm::quat extractRotationFromMatrix(const glm::mat4& matrix) {
        glm::mat3 rotMat(matrix);
        
        // Remove scale
        glm::vec3 scale = extractScaleFromMatrix(matrix);
        rotMat[0] /= scale.x;
        rotMat[1] /= scale.y;
        rotMat[2] /= scale.z;
        
        return glm::quat_cast(rotMat);
    }

    EntityID id;
    TransformStorage* transformStorage;
};
