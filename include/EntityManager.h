#pragma once

#include "Entity.h"
#include "TransformStorage.h"
#include <memory>

/// EntityManager manages entities and their transform components
/// Uses SOA TransformStorage for efficient memory layout
class EntityManager {
public:
    EntityManager() = default;

    /// Create a new entity with a transform component
    /// Returns an Entity handle with access to TransformStorage
    Entity createEntity() {
        auto id = transformStorage.allocate();
        return Entity(id, &transformStorage);
    }

    /// Destroy an entity and free its transform data
    void destroyEntity(Entity& entity) {
        if (entity.isValid()) {
            transformStorage.deallocate(entity.getID());
            entity.id = TransformStorage::INVALID_ENTITY;
        }
    }

    /// Get the transform storage (for advanced usage)
    TransformStorage& getTransformStorage() {
        return transformStorage;
    }

    const TransformStorage& getTransformStorage() const {
        return transformStorage;
    }

    /// Get the number of entities
    size_t getEntityCount() const {
        return transformStorage.size();
    }

private:
    TransformStorage transformStorage;
};
