#pragma once

#include "TransformStorage.h"
#include "CollisionStorage.h"
#include "RenderStorage.h"

/// EntityContext holds all the data that entities need to access
/// This allows for easy extension with additional systems/storages
struct EntityContext {
    TransformStorage* transformStorage = nullptr;
    CollisionStorage* collisionStorage = nullptr;
    RenderStorage* renderStorage = nullptr;
    
    // Future: Add other storage systems here
    // e.g., PhysicsStorage* physicsStorage = nullptr;
};
