#pragma once

#include "EntityContext.h"
#include "Transform.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

class EntityManager;

/// Entity class that encapsulates transform operations
/// Acts as a handle to transform data stored in EntityContext
/// Can only be created through EntityManager
class Entity {
public:
    using EntityID = TransformStorage::EntityID;

    /// Default constructor is deleted - entities must be created through EntityManager
    Entity() = delete;

    /// Check if this entity is valid
    bool isValid() const {
        return id != TransformStorage::INVALID_ENTITY && 
               context != nullptr &&
               context->transformStorage != nullptr &&
               context->transformStorage->isValid(id);
    }

    /// Get the entity ID
    EntityID getID() const {
        return id;
    }

    // Transform property accessors - encapsulated as if Entity owns the data
    
    /// Get the position
    glm::vec3 getPosition() const {
        if (!isValid()) return glm::vec3(0.0f);
        return context->transformStorage->getPosition(id);
    }

    /// Set the position
    void setPosition(const glm::vec3& pos) {
        if (isValid()) {
            context->transformStorage->setPosition(id, pos);
        }
    }

    /// Get the rotation
    glm::quat getRotation() const {
        if (!isValid()) return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        return context->transformStorage->getRotation(id);
    }

    /// Set the rotation
    void setRotation(const glm::quat& rot) {
        if (isValid()) {
            context->transformStorage->setRotation(id, rot);
        }
    }

    /// Get the scale
    glm::vec3 getScale() const {
        if (!isValid()) return glm::vec3(1.0f);
        return context->transformStorage->getScale(id);
    }

    /// Set the scale
    void setScale(const glm::vec3& scale) {
        if (isValid()) {
            context->transformStorage->setScale(id, scale);
        }
    }

    // Parent-child relationship methods
    
    /// Get the parent entity
    Entity getParent() const {
        if (!isValid()) return createInvalidEntity();
        EntityID parentId = context->transformStorage->getParent(id);
        if (parentId == TransformStorage::INVALID_ENTITY) return createInvalidEntity();
        return createEntity(parentId, context);
    }

    /// Set the parent entity
    void setParent(const Entity& parent) {
        if (!isValid()) return;
        
        if (parent.isValid() && parent.context == context) {
            context->transformStorage->setParent(id, parent.getID());
        } else {
            // Remove parent
            context->transformStorage->setParent(id, TransformStorage::INVALID_ENTITY);
        }
    }

    /// Check if this entity has a parent
    bool hasParent() const {
        if (!isValid()) return false;
        return context->transformStorage->getParent(id) != TransformStorage::INVALID_ENTITY;
    }

    /// Get all child entities
    std::vector<Entity> getChildren() const {
        std::vector<Entity> result;
        if (!isValid()) return result;
        
        const auto& childIds = context->transformStorage->getChildren(id);
        result.reserve(childIds.size());
        
        for (EntityID childId : childIds) {
            result.push_back(createEntity(childId, context));
        }
        
        return result;
    }

    /// Add a child entity
    void addChild(const Entity& child) {
        if (!isValid() || !child.isValid()) return;
        if (context != child.context) return;
        
        context->transformStorage->addChild(id, child.getID());
    }

    /// Remove a child entity (only removes the relationship, doesn't delete the entity)
    void removeChild(const Entity& child) {
        if (!isValid() || !child.isValid()) return;
        if (context != child.context) return;
        
        // setParent will handle removing from parent's children list
        context->transformStorage->setParent(child.getID(), TransformStorage::INVALID_ENTITY);
    }

    /// Get the number of children
    size_t getChildCount() const {
        if (!isValid()) return 0;
        return context->transformStorage->getChildren(id).size();
    }

    // World transform matrix methods

    /// Get the world transform matrix (computed from hierarchy)
    glm::mat4 getWorldMatrix() const {
        if (!isValid()) return glm::mat4(1.0f);
        return context->transformStorage->getWorldMatrix(id);
    }

    /// Update the world transform matrix for this entity
    /// Computes from local transform and parent's world matrix
    void updateTransform() {
        if (isValid()) {
            context->transformStorage->updateWorldMatrix(id);
        }
    }

    /// Update the world transform matrix for this entity and all its children recursively
    void updateTransformHierarchy() {
        if (isValid()) {
            context->transformStorage->updateWorldMatrixHierarchy(id);
        }
    }

    // Transform accessor

    /// Get a Transform accessor for this entity
    /// Allows reading/writing both local (relative to parent) and world (absolute) transforms
    Transform getTransform() {
        if (!isValid()) {
            return Transform(TransformStorage::INVALID_ENTITY, nullptr);
        }
        return Transform(id, context->transformStorage);
    }

    /// Get a Transform accessor for this entity (const version)
    Transform getTransform() const {
        if (!isValid()) {
            return Transform(TransformStorage::INVALID_ENTITY, nullptr);
        }
        return Transform(id, context->transformStorage);
    }

    /// Delete the transform (marks it for reuse)
    void deleteTransform() {
        if (isValid()) {
            context->transformStorage->deallocate(id);
            id = TransformStorage::INVALID_ENTITY;
        }
    }

private:
    /// Private constructor - can only be created by EntityManager
    Entity(EntityID id, EntityContext* ctx)
        : id(id), context(ctx) {}

    /// Helper method to create Entity instances from within Entity methods
    static Entity createEntity(EntityID id, EntityContext* ctx) {
        return Entity(id, ctx);
    }

    /// Helper method to create an invalid Entity
    static Entity createInvalidEntity() {
        return Entity(TransformStorage::INVALID_ENTITY, nullptr);
    }

    EntityID id;
    EntityContext* context;
    
    friend class EntityManager;
};
