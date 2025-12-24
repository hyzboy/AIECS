#pragma once

#include "TransformStorage.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class EntityManager;

/// Entity class that encapsulates transform operations
/// Acts as a handle to transform data stored in TransformStorage
class Entity {
public:
    using EntityID = TransformStorage::EntityID;

    Entity() : id(TransformStorage::INVALID_ENTITY), transformStorage(nullptr) {}
    
    Entity(EntityID id, TransformStorage* storage)
        : id(id), transformStorage(storage) {}

    /// Check if this entity is valid
    bool isValid() const {
        return id != TransformStorage::INVALID_ENTITY && 
               transformStorage != nullptr &&
               transformStorage->isValid(id);
    }

    /// Get the entity ID
    EntityID getID() const {
        return id;
    }

    // Transform property accessors - encapsulated as if Entity owns the data
    
    /// Get the position
    glm::vec3 getPosition() const {
        if (!isValid()) return glm::vec3(0.0f);
        return transformStorage->getPosition(id);
    }

    /// Set the position
    void setPosition(const glm::vec3& pos) {
        if (isValid()) {
            transformStorage->setPosition(id, pos);
        }
    }

    /// Get the rotation
    glm::quat getRotation() const {
        if (!isValid()) return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        return transformStorage->getRotation(id);
    }

    /// Set the rotation
    void setRotation(const glm::quat& rot) {
        if (isValid()) {
            transformStorage->setRotation(id, rot);
        }
    }

    /// Get the scale
    glm::vec3 getScale() const {
        if (!isValid()) return glm::vec3(1.0f);
        return transformStorage->getScale(id);
    }

    /// Set the scale
    void setScale(const glm::vec3& scale) {
        if (isValid()) {
            transformStorage->setScale(id, scale);
        }
    }

    /// Delete the transform (marks it for reuse)
    void deleteTransform() {
        if (isValid()) {
            transformStorage->deallocate(id);
            id = TransformStorage::INVALID_ENTITY;
        }
    }

private:
    EntityID id;
    TransformStorage* transformStorage;
    
    friend class EntityManager;
};
