#pragma once

#include "Object.h"
#include "Module.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

class EventSystem;

/// World manages all game objects and modules
/// Acts as the main container for the game simulation
class World : public Object {
public:
    World(const std::string& name = "World");
    ~World() override;

    /// Initialize the world
    void initialize();

    /// Shut down the world
    void shutdown();

    /// Update all systems and objects
    void update(float deltaTime);

    /// Create a new object in the world
    template<typename T, typename... Args>
    std::shared_ptr<T> createObject(Args&&... args) {
        auto obj = std::make_shared<T>(std::forward<Args>(args)...);
        objects.push_back(obj);
        obj->onCreate();
        return obj;
    }

    /// Register a module
    template<typename T, typename... Args>
    std::shared_ptr<T> registerModule(Args&&... args) {
        auto module = std::make_shared<T>(std::forward<Args>(args)...);
        modules[typeid(T).name()] = module;
        return module;
    }

    /// Get a module by type
    template<typename T>
    std::shared_ptr<T> getModule() const {
        auto it = modules.find(typeid(T).name());
        if (it != modules.end()) {
            return std::static_pointer_cast<T>(it->second);
        }
        return nullptr;
    }

    /// Get event system
    std::shared_ptr<EventSystem> getEventSystem() const;

    /// Get object count
    size_t getObjectCount() const { return objects.size(); }

    /// Check if world is active
    bool isActive() const { return active; }

private:
    std::vector<std::shared_ptr<Object>> objects;
    std::unordered_map<std::string, std::shared_ptr<Module>> modules;
    bool active = false;
};
