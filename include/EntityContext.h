#pragma once

#include "TransformStorage.h"

/// EntityContext holds all the data that entities need to access
/// This allows for easy extension with additional systems/storages
struct EntityContext {
    TransformStorage* transformStorage = nullptr;
    
    // Future: Add other storage systems here
    // e.g., RenderStorage* renderStorage = nullptr;
    //       PhysicsStorage* physicsStorage = nullptr;
};
