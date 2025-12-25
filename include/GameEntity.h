#pragma once

#include "Object.h"
#include "EntityComponent.h"
#include <memory>
#include <unordered_map>
#include <typeinfo>
#include <string>

/// GameEntity - represents game objects with components
/// This replaces the Entity class in the Frostbite architecture
class GameEntity : public Object {
public:
    explicit GameEntity(const std::string& name = "GameEntity");
    ~GameEntity() override;

    /// Add component to entity
    template<typename T, typename... Args>
    std::shared_ptr<T> addComponent(Args&&... args) {
        auto component = std::make_shared<T>(std::forward<Args>(args)...);
        components[typeid(T).hash_code()] = component;
        component->onAttach();
        return component;
    }

    /// Get component by type
    template<typename T>
    std::shared_ptr<T> getComponent() const {
        auto it = components.find(typeid(T).hash_code());
        if (it != components.end()) {
            return std::static_pointer_cast<T>(it->second);
        }
        return nullptr;
    }

    /// Check if entity has component
    template<typename T>
    bool hasComponent() const {
        return components.find(typeid(T).hash_code()) != components.end();
    }

    /// Remove component by type
    template<typename T>
    void removeComponent() {
        auto it = components.find(typeid(T).hash_code());
        if (it != components.end()) {
            it->second->onDetach();
            components.erase(it);
        }
    }

    /// Update all components
    void onUpdate(float deltaTime) override;

    /// Get component count
    size_t getComponentCount() const { return components.size(); }

private:
    // Use hash_code instead of string for faster lookups
    // hash_code is much cheaper than string comparison
    std::unordered_map<std::size_t, std::shared_ptr<EntityComponent>> components;
};
