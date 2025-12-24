#pragma once

#include "Object.h"

/// Base class for all engine modules
/// Modules are singleton-like systems that handle specific functionalities
class Module : public Object {
public:
    explicit Module(const std::string& name = "Module");
    virtual ~Module() = default;

    /// Initialize the module
    virtual void initialize() {}

    /// Shut down the module
    virtual void shutdown() {}

    /// Update the module (called once per frame)
    virtual void update(float deltaTime) {}

    /// Get if module is initialized
    bool isInitialized() const { return initialized; }

protected:
    bool initialized = false;
};
