#include "World.h"
#include "EventSystem.h"

World::World(const std::string& name)
    : Object(name), active(false) {
    // Register EventSystem by default
    registerModule<EventSystem>();
}

World::~World() {
    shutdown();
}

void World::initialize() {
    // Initialize all modules
    for (auto& module : modules) {
        module.second->initialize();
    }
    
    active = true;
    onCreate();
}

void World::shutdown() {
    // Destroy all objects
    objects.clear();
    
    // Shutdown all modules
    for (auto& module : modules) {
        module.second->shutdown();
    }
    modules.clear();
    
    active = false;
    onDestroy();
}

void World::update(float deltaTime) {
    if (!active) return;
    
    // Update all modules
    for (auto& module : modules) {
        module.second->update(deltaTime);
    }
    
    // Update all objects
    for (auto& object : objects) {
        object->onUpdate(deltaTime);
    }
}

std::shared_ptr<EventSystem> World::getEventSystem() const {
    return getModule<EventSystem>();
}
