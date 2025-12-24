#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include "EntityManager.h"

int main() {
    std::cout << "AIECS - C++20 ECS Project with Component-Based Architecture" << std::endl;
    std::cout << "==========================================================" << std::endl;
    std::cout << std::endl;

    // Create EntityManager
    EntityManager entityManager;
    std::cout << "EntityManager created!" << std::endl;
    std::cout << std::endl;

    // Create entities
    std::cout << "Creating entities..." << std::endl;
    Entity entity1 = entityManager.createEntity();
    Entity entity2 = entityManager.createEntity();
    Entity entity3 = entityManager.createEntity();
    std::cout << "Created 3 entities (IDs: " 
              << entity1.getID() << ", " 
              << entity2.getID() << ", " 
              << entity3.getID() << ")" << std::endl;
    std::cout << std::endl;

    // Add components to entities
    std::cout << "Adding components to entities..." << std::endl;
    entityManager.addTransformComponent(entity1);
    entityManager.addRenderComponent(entity1);
    
    entityManager.addTransformComponent(entity2);
    entityManager.addCollisionComponent(entity2);
    
    entityManager.addTransformComponent(entity3);
    entityManager.addRenderComponent(entity3);
    entityManager.addCollisionComponent(entity3);
    
    std::cout << "Entity 1: Transform + Render" << std::endl;
    std::cout << "Entity 2: Transform + Collision" << std::endl;
    std::cout << "Entity 3: Transform + Render + Collision" << std::endl;
    std::cout << std::endl;

    // Test TransformComponent
    std::cout << "=== Testing TransformComponent ===" << std::endl;
    std::cout << std::endl;
    
    auto transform1 = entity1.getTransformComponent();
    if (transform1) {
        transform1->setLocalPosition(glm::vec3(10.0f, 5.0f, 2.0f));
        transform1->setLocalScale(glm::vec3(2.0f, 2.0f, 2.0f));
        
        std::cout << "Entity 1 Transform:" << std::endl;
        std::cout << "  Position: (" 
                  << transform1->getLocalPosition().x << ", "
                  << transform1->getLocalPosition().y << ", "
                  << transform1->getLocalPosition().z << ")" << std::endl;
        std::cout << "  Scale: ("
                  << transform1->getLocalScale().x << ", "
                  << transform1->getLocalScale().y << ", "
                  << transform1->getLocalScale().z << ")" << std::endl;
    }
    std::cout << std::endl;

    // Test RenderComponent
    std::cout << "=== Testing RenderComponent ===" << std::endl;
    std::cout << std::endl;
    
    auto render1 = entity1.getRenderComponent();
    if (render1) {
        render1->setMeshName("cube.mesh");
        render1->setMaterialName("metal.mat");
        render1->setVisible(true);
        render1->setCastShadows(true);
        
        std::cout << "Entity 1 Render:" << std::endl;
        std::cout << "  Mesh: " << render1->getMeshName() << std::endl;
        std::cout << "  Material: " << render1->getMaterialName() << std::endl;
        std::cout << "  Visible: " << (render1->isVisible() ? "Yes" : "No") << std::endl;
        std::cout << "  Casts Shadows: " << (render1->castsShadows() ? "Yes" : "No") << std::endl;
    }
    std::cout << std::endl;

    // Test CollisionComponent
    std::cout << "=== Testing CollisionComponent ===" << std::endl;
    std::cout << std::endl;
    
    auto collision2 = entity2.getCollisionComponent();
    if (collision2) {
        collision2->setBoundingBox(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        collision2->setCollisionLayer(1);
        collision2->setEnabled(true);
        
        std::cout << "Entity 2 Collision:" << std::endl;
        std::cout << "  Bounding Box Min: ("
                  << collision2->getBoundingBoxMin().x << ", "
                  << collision2->getBoundingBoxMin().y << ", "
                  << collision2->getBoundingBoxMin().z << ")" << std::endl;
        std::cout << "  Bounding Box Max: ("
                  << collision2->getBoundingBoxMax().x << ", "
                  << collision2->getBoundingBoxMax().y << ", "
                  << collision2->getBoundingBoxMax().z << ")" << std::endl;
        std::cout << "  Collision Layer: " << collision2->getCollisionLayer() << std::endl;
        std::cout << "  Enabled: " << (collision2->isEnabled() ? "Yes" : "No") << std::endl;
    }
    std::cout << std::endl;

    // Test component checks
    std::cout << "=== Component Presence Check ===" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Entity 1 components:" << std::endl;
    std::cout << "  Has Transform: " << (entity1.hasTransformComponent() ? "Yes" : "No") << std::endl;
    std::cout << "  Has Collision: " << (entity1.hasCollisionComponent() ? "Yes" : "No") << std::endl;
    std::cout << "  Has Render: " << (entity1.hasRenderComponent() ? "Yes" : "No") << std::endl;
    std::cout << std::endl;

    std::cout << "Entity 3 (all components):" << std::endl;
    std::cout << "  Has Transform: " << (entity3.hasTransformComponent() ? "Yes" : "No") << std::endl;
    std::cout << "  Has Collision: " << (entity3.hasCollisionComponent() ? "Yes" : "No") << std::endl;
    std::cout << "  Has Render: " << (entity3.hasRenderComponent() ? "Yes" : "No") << std::endl;
    std::cout << std::endl;

    // Test component removal
    std::cout << "=== Testing Component Removal ===" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Removing Collision from Entity 3..." << std::endl;
    entityManager.removeCollisionComponent(entity3);
    std::cout << "Entity 3 has Collision: " << (entity3.hasCollisionComponent() ? "Yes" : "No") << std::endl;
    std::cout << std::endl;

    std::cout << "Total entities: " << entityManager.getEntityCount() << std::endl;
    std::cout << std::endl;

    std::cout << "Component-based ECS system is working correctly!" << std::endl;

    return 0;
}
