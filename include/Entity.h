#pragma once

#include "TransformStorage.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

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

    // Parent-child relationship methods
    
    /// Get the parent entity
    Entity getParent() const {
        if (!isValid()) return Entity();
        EntityID parentId = transformStorage->getParent(id);
        if (parentId == TransformStorage::INVALID_ENTITY) return Entity();
        return Entity(parentId, transformStorage);
    }

    /// Set the parent entity
    void setParent(const Entity& parent) {
        if (!isValid()) return;
        
        if (parent.isValid() && parent.transformStorage == transformStorage) {
            transformStorage->setParent(id, parent.getID());
        } else {
            // Remove parent
            transformStorage->setParent(id, TransformStorage::INVALID_ENTITY);
        }
    }

    /// Check if this entity has a parent
    bool hasParent() const {
        if (!isValid()) return false;
        return transformStorage->getParent(id) != TransformStorage::INVALID_ENTITY;
    }

    /// Get all child entities
    std::vector<Entity> getChildren() const {
        std::vector<Entity> result;
        if (!isValid()) return result;
        
        const auto& childIds = transformStorage->getChildren(id);
        result.reserve(childIds.size());
        
        for (EntityID childId : childIds) {
            result.emplace_back(childId, transformStorage);
        }
        
        return result;
    }

    /// Add a child entity
    void addChild(const Entity& child) {
        if (!isValid() || !child.isValid()) return;
        if (transformStorage != child.transformStorage) return;
        
        transformStorage->addChild(id, child.getID());
    }

    /// Remove a child entity (only removes the relationship, doesn't delete the entity)
    void removeChild(const Entity& child) {
        if (!isValid() || !child.isValid()) return;
        if (transformStorage != child.transformStorage) return;
        
        transformStorage->removeChild(id, child.getID());
        // Also clear the child's parent reference
        transformStorage->setParent(child.getID(), TransformStorage::INVALID_ENTITY);
    }

    /// Get the number of children
    size_t getChildCount() const {
        if (!isValid()) return 0;
        return transformStorage->getChildren(id).size();
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
