#pragma once

#include "Entity.h"
#include "EntityContext.h"
#include "TransformStorage.h"
#include "CollisionStorage.h"
#include "RenderStorage.h"
#include <memory>
#include <vector>

/// EntityManager manages entities and their components
/// Uses SOA storage for each component type
class EntityManager {
public:
    EntityManager() {
        // Initialize the context with pointers to the storage systems
        context.transformStorage = &transformStorage;
        context.collisionStorage = &collisionStorage;
        context.renderStorage = &renderStorage;
    }

    /// Create a new entity
    /// Returns an Entity handle with access to EntityContext
    /// Entity starts with no components attached
    Entity createEntity() {
        Entity::EntityID entityId = nextEntityId++;
        Entity entity(entityId, &context);
        entities.push_back(entity);
        return entity;
    }

    /// Add TransformComponent to an entity
    void addTransformComponent(Entity& entity) {
        if (!entity.isValid() || entity.hasTransformComponent()) return;
        
        ComponentID compId = transformStorage.allocate();
        entity.transformComponentId = compId;
    }

    /// Add CollisionComponent to an entity
    void addCollisionComponent(Entity& entity) {
        if (!entity.isValid() || entity.hasCollisionComponent()) return;
        
        ComponentID compId = collisionStorage.allocate();
        entity.collisionComponentId = compId;
    }

    /// Add RenderComponent to an entity
    void addRenderComponent(Entity& entity) {
        if (!entity.isValid() || entity.hasRenderComponent()) return;
        
        ComponentID compId = renderStorage.allocate();
        entity.renderComponentId = compId;
    }

    /// Remove TransformComponent from an entity
    void removeTransformComponent(Entity& entity) {
        if (!entity.isValid() || !entity.hasTransformComponent()) return;
        
        transformStorage.deallocate(entity.transformComponentId);
        entity.transformComponentId = INVALID_COMPONENT;
    }

    /// Remove CollisionComponent from an entity
    void removeCollisionComponent(Entity& entity) {
        if (!entity.isValid() || !entity.hasCollisionComponent()) return;
        
        collisionStorage.deallocate(entity.collisionComponentId);
        entity.collisionComponentId = INVALID_COMPONENT;
    }

    /// Remove RenderComponent from an entity
    void removeRenderComponent(Entity& entity) {
        if (!entity.isValid() || !entity.hasRenderComponent()) return;
        
        renderStorage.deallocate(entity.renderComponentId);
        entity.renderComponentId = INVALID_COMPONENT;
    }

    /// Destroy an entity and free all its components
    void destroyEntity(Entity& entity) {
        if (!entity.isValid()) return;
        
        // Remove all components
        if (entity.hasTransformComponent()) {
            transformStorage.deallocate(entity.transformComponentId);
        }
        if (entity.hasCollisionComponent()) {
            collisionStorage.deallocate(entity.collisionComponentId);
        }
        if (entity.hasRenderComponent()) {
            renderStorage.deallocate(entity.renderComponentId);
        }
        
        entity.entityId = Entity::INVALID_ENTITY;
    }

    /// Update all transform hierarchies
    void updateAllTransforms() {
        for (size_t i = 0; i < transformStorage.size(); ++i) {
            // Update only root entities (those without parents)
            if (transformStorage.isValid(i) && 
                transformStorage.getParent(i) == TransformStorage::INVALID_ENTITY) {
                transformStorage.updateWorldMatrixHierarchy(i);
            }
        }
    }

    /// Get the transform storage (for advanced usage)
    TransformStorage& getTransformStorage() {
        return transformStorage;
    }

    const TransformStorage& getTransformStorage() const {
        return transformStorage;
    }

    /// Get the collision storage (for advanced usage)
    CollisionStorage& getCollisionStorage() {
        return collisionStorage;
    }

    const CollisionStorage& getCollisionStorage() const {
        return collisionStorage;
    }

    /// Get the render storage (for advanced usage)
    RenderStorage& getRenderStorage() {
        return renderStorage;
    }

    const RenderStorage& getRenderStorage() const {
        return renderStorage;
    }

    /// Get the number of entities
    size_t getEntityCount() const {
        return entities.size();
    }

private:
    TransformStorage transformStorage;
    CollisionStorage collisionStorage;
    RenderStorage renderStorage;
    EntityContext context;
    
    std::vector<Entity> entities;
    Entity::EntityID nextEntityId = 0;
};
