#pragma once

#include "EntityContext.h"
#include "ComponentTypes.h"
#include "TransformComponent.h"
#include "CollisionComponent.h"
#include "RenderComponent.h"
#include <optional>

class EntityManager;

/// Entity class - lightweight handle that stores only component indices
/// Can only be created through EntityManager
/// All data access must go through component accessors
class Entity {
public:
    using EntityID = uint32_t;
    static constexpr EntityID INVALID_ENTITY = UINT32_MAX;

    /// Default constructor is deleted - entities must be created through EntityManager
    Entity() = delete;

    /// Check if this entity is valid
    bool isValid() const {
        return entityId != INVALID_ENTITY && context != nullptr;
    }

    /// Get the entity ID
    EntityID getID() const {
        return entityId;
    }

    // Component accessors - returns std::optional with component if it exists

    /// Get TransformComponent if entity has one
    std::optional<TransformComponent> getTransformComponent() {
        if (!isValid() || transformComponentId == INVALID_COMPONENT) {
            return std::nullopt;
        }
        return TransformComponent(transformComponentId, context->transformStorage);
    }

    /// Get CollisionComponent if entity has one
    std::optional<CollisionComponent> getCollisionComponent() {
        if (!isValid() || collisionComponentId == INVALID_COMPONENT) {
            return std::nullopt;
        }
        return CollisionComponent(collisionComponentId, context->collisionStorage);
    }

    /// Get RenderComponent if entity has one
    std::optional<RenderComponent> getRenderComponent() {
        if (!isValid() || renderComponentId == INVALID_COMPONENT) {
            return std::nullopt;
        }
        return RenderComponent(renderComponentId, context->renderStorage);
    }

    /// Check if entity has a TransformComponent
    bool hasTransformComponent() const {
        return transformComponentId != INVALID_COMPONENT;
    }

    /// Check if entity has a CollisionComponent
    bool hasCollisionComponent() const {
        return collisionComponentId != INVALID_COMPONENT;
    }

    /// Check if entity has a RenderComponent
    bool hasRenderComponent() const {
        return renderComponentId != INVALID_COMPONENT;
    }

    /// Get transform component ID
    ComponentID getTransformComponentID() const {
        return transformComponentId;
    }

    /// Get collision component ID
    ComponentID getCollisionComponentID() const {
        return collisionComponentId;
    }

    /// Get render component ID
    ComponentID getRenderComponentID() const {
        return renderComponentId;
    }

private:
    /// Private constructor - can only be created by EntityManager
    Entity(EntityID entId, EntityContext* ctx)
        : entityId(entId)
        , context(ctx)
        , transformComponentId(INVALID_COMPONENT)
        , collisionComponentId(INVALID_COMPONENT)
        , renderComponentId(INVALID_COMPONENT) {}

    EntityID entityId;
    EntityContext* context;
    
    // Component indices
    ComponentID transformComponentId;
    ComponentID collisionComponentId;
    ComponentID renderComponentId;
    
    friend class EntityManager;
};
