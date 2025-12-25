#pragma once

#include <memory>
#include <string>

/// Base component class for GameEntity
class GameEntity; // Forward declaration

class EntityComponent {
public:
    explicit EntityComponent(const std::string& name = "EntityComponent")
        : componentName(name) {}
    virtual ~EntityComponent() = default;

    /// Called when component is attached
    virtual void onAttach() {}

    /// Called each frame
    virtual void onUpdate(float deltaTime) {}

    /// Called when component is detached
    virtual void onDetach() {}

    const std::string& getName() const { return componentName; }

    /// Set the owner entity
    void setOwner(std::shared_ptr<GameEntity> entity) { owner = entity; }

    /// Get the owner entity
    std::shared_ptr<GameEntity> getOwner() const { return owner.lock(); }

protected:
    std::string componentName;
    std::weak_ptr<GameEntity> owner;
};
