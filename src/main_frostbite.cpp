#include <iostream>
#include <glm/glm.hpp>
#include "World.h"
#include "GameEntity.h"
#include "TransformComponentFB.h"
#include "CollisionComponentFB.h"
#include "RenderComponentFB.h"
#include "EventSystem.h"

// Custom event example
struct EntityCreatedEvent : public EventData {
    explicit EntityCreatedEvent(uint64_t entityId)
        : EventData("EntityCreated"), entityId(entityId) {}
    uint64_t entityId;
};

int main() {
    std::cout << "Frostbite-Style Architecture Demo" << std::endl;
    std::cout << "===================================" << std::endl << std::endl;

    // Create the world
    auto world = std::make_shared<World>("GameWorld");
    std::cout << "World created!" << std::endl << std::endl;

    // Initialize the world
    world->initialize();
    std::cout << "World initialized!" << std::endl;
    std::cout << "Event system registered and active" << std::endl << std::endl;

    // Get event system
    auto eventSystem = world->getEventSystem();
    
    // Subscribe to entity creation events
    eventSystem->subscribe("EntityCreated", [](const std::shared_ptr<EventData>& event) {
        auto createEvent = std::dynamic_pointer_cast<EntityCreatedEvent>(event);
        if (createEvent) {
            std::cout << "[EVENT] Entity created with ID: " << createEvent->entityId << std::endl;
        }
    });

    // Create game entities
    std::cout << "Creating game entities..." << std::endl;
    auto entity1 = world->createObject<GameEntity>("Player");
    auto entity2 = world->createObject<GameEntity>("Enemy");
    auto entity3 = world->createObject<GameEntity>("Obstacle");
    std::cout << "Created 3 entities" << std::endl << std::endl;

    // Add components to entity1 (Player)
    std::cout << "=== Setting up Player ===" << std::endl;
    auto transform1 = entity1->addComponent<TransformComponentFB>("PlayerTransform");
    auto render1 = entity1->addComponent<RenderComponentFB>("PlayerRender");
    
    transform1->setLocalPosition(glm::vec3(10.0f, 5.0f, 2.0f));
    transform1->setLocalScale(glm::vec3(2.0f, 2.0f, 2.0f));
    render1->setMeshName("player.mesh");
    render1->setMaterialName("player.mat");
    render1->setVisible(true);
    
    std::cout << "Player - Position: (" 
              << transform1->getLocalPosition().x << ", "
              << transform1->getLocalPosition().y << ", "
              << transform1->getLocalPosition().z << ")" << std::endl;
    std::cout << "Player - Mesh: " << render1->getMeshName() << std::endl << std::endl;

    // Add components to entity2 (Enemy)
    std::cout << "=== Setting up Enemy ===" << std::endl;
    auto transform2 = entity2->addComponent<TransformComponentFB>("EnemyTransform");
    auto collision2 = entity2->addComponent<CollisionComponentFB>("EnemyCollision");
    
    transform2->setLocalPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    collision2->setBoundingBox(glm::vec3(-1.0f), glm::vec3(1.0f));
    collision2->setCollisionLayer(1);
    
    std::cout << "Enemy - Position: (" 
              << transform2->getLocalPosition().x << ", "
              << transform2->getLocalPosition().y << ", "
              << transform2->getLocalPosition().z << ")" << std::endl;
    std::cout << "Enemy - Collision Layer: " << collision2->getCollisionLayer() << std::endl << std::endl;

    // Add components to entity3 (Obstacle) and set as child of player
    std::cout << "=== Setting up Obstacle ===" << std::endl;
    auto transform3 = entity3->addComponent<TransformComponentFB>("ObstacleTransform");
    auto render3 = entity3->addComponent<RenderComponentFB>("ObstacleRender");
    
    transform3->setLocalPosition(glm::vec3(5.0f, 0.0f, 0.0f));
    transform3->setParent(entity1);  // Make it child of player
    transform1->addChild(entity3);
    
    render3->setMeshName("obstacle.mesh");
    
    std::cout << "Obstacle - Local Position: (" 
              << transform3->getLocalPosition().x << ", "
              << transform3->getLocalPosition().y << ", "
              << transform3->getLocalPosition().z << ")" << std::endl;
    std::cout << "Obstacle - World Position: (" 
              << transform3->getWorldPosition().x << ", "
              << transform3->getWorldPosition().y << ", "
              << transform3->getWorldPosition().z << ")" << std::endl << std::endl;

    // Dispatch entity creation events
    std::cout << "=== Dispatching Events ===" << std::endl;
    eventSystem->dispatch(std::make_shared<EntityCreatedEvent>(entity1->getID()));
    eventSystem->dispatch(std::make_shared<EntityCreatedEvent>(entity2->getID()));
    eventSystem->dispatch(std::make_shared<EntityCreatedEvent>(entity3->getID()));
    std::cout << std::endl;

    // Test world update
    std::cout << "=== Updating World ===" << std::endl;
    const float deltaTime = 1.0f / 60.0f;  // 60 FPS
    
    for (int i = 0; i < 3; ++i) {
        std::cout << "Update frame " << (i + 1) << std::endl;
        world->update(deltaTime);
    }
    std::cout << std::endl;

    // Component presence check
    std::cout << "=== Component Checks ===" << std::endl;
    std::cout << "Player components:" << std::endl;
    std::cout << "  Has Transform: " << (entity1->hasComponent<TransformComponentFB>() ? "Yes" : "No") << std::endl;
    std::cout << "  Has Collision: " << (entity1->hasComponent<CollisionComponentFB>() ? "Yes" : "No") << std::endl;
    std::cout << "  Has Render: " << (entity1->hasComponent<RenderComponentFB>() ? "Yes" : "No") << std::endl << std::endl;

    std::cout << "Enemy components:" << std::endl;
    std::cout << "  Has Transform: " << (entity2->hasComponent<TransformComponentFB>() ? "Yes" : "No") << std::endl;
    std::cout << "  Has Collision: " << (entity2->hasComponent<CollisionComponentFB>() ? "Yes" : "No") << std::endl;
    std::cout << "  Has Render: " << (entity2->hasComponent<RenderComponentFB>() ? "Yes" : "No") << std::endl << std::endl;

    // Remove component example
    std::cout << "=== Removing Components ===" << std::endl;
    std::cout << "Removing Collision from Enemy..." << std::endl;
    entity2->removeComponent<CollisionComponentFB>();
    std::cout << "Enemy - Has Collision: " << (entity2->hasComponent<CollisionComponentFB>() ? "Yes" : "No") << std::endl << std::endl;

    // Summary
    std::cout << "Total objects in world: " << world->getObjectCount() << std::endl;
    std::cout << std::endl;
    std::cout << "Frostbite-style ECS architecture demonstration complete!" << std::endl;

    // Shutdown
    world->shutdown();
    std::cout << "World shut down successfully!" << std::endl;

    return 0;
}
