#include "GameEntity.h"

GameEntity::GameEntity(const std::string& name)
    : Object(name) {
}

GameEntity::~GameEntity() {
    // Detach all components
    for (auto& component : components) {
        component.second->onDetach();
    }
    components.clear();
}

void GameEntity::onUpdate(float deltaTime) {
    for (auto& component : components) {
        component.second->onUpdate(deltaTime);
    }
}
