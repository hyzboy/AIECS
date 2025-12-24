#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include "EntityManager.h"

int main() {
    std::cout << "AIECS - C++20 ECS Project with SOA Transform Storage" << std::endl;
    std::cout << "====================================================" << std::endl;
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

    // Set transform properties on entities (encapsulated as if Entity owns the data)
    std::cout << "Setting transform properties..." << std::endl;
    
    entity1.setPosition(glm::vec3(1.0f, 2.0f, 3.0f));
    entity1.setScale(glm::vec3(2.0f, 2.0f, 2.0f));
    std::cout << "Entity 1 - Position: (" 
              << entity1.getPosition().x << ", " 
              << entity1.getPosition().y << ", " 
              << entity1.getPosition().z << ")" << std::endl;
    std::cout << "Entity 1 - Scale: (" 
              << entity1.getScale().x << ", " 
              << entity1.getScale().y << ", " 
              << entity1.getScale().z << ")" << std::endl;

    entity2.setPosition(glm::vec3(5.0f, 0.0f, -3.0f));
    entity2.setRotation(glm::angleAxis(glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
    std::cout << "Entity 2 - Position: (" 
              << entity2.getPosition().x << ", " 
              << entity2.getPosition().y << ", " 
              << entity2.getPosition().z << ")" << std::endl;

    entity3.setPosition(glm::vec3(-2.0f, 4.0f, 1.0f));
    entity3.setScale(glm::vec3(0.5f, 0.5f, 0.5f));
    std::cout << "Entity 3 - Position: (" 
              << entity3.getPosition().x << ", " 
              << entity3.getPosition().y << ", " 
              << entity3.getPosition().z << ")" << std::endl;
    std::cout << "Entity 3 - Scale: (" 
              << entity3.getScale().x << ", " 
              << entity3.getScale().y << ", " 
              << entity3.getScale().z << ")" << std::endl;
    std::cout << std::endl;

    // Demonstrate transform deletion
    std::cout << "Deleting entity 2's transform..." << std::endl;
    entity2.deleteTransform();
    std::cout << "Entity 2 valid: " << (entity2.isValid() ? "Yes" : "No") << std::endl;
    std::cout << std::endl;

    // Create a new entity (should reuse entity 2's slot)
    Entity entity4 = entityManager.createEntity();
    std::cout << "Created entity 4 (ID: " << entity4.getID() << ", should reuse freed slot)" << std::endl;
    entity4.setPosition(glm::vec3(10.0f, 10.0f, 10.0f));
    std::cout << "Entity 4 - Position: (" 
              << entity4.getPosition().x << ", " 
              << entity4.getPosition().y << ", " 
              << entity4.getPosition().z << ")" << std::endl;
    std::cout << std::endl;

    std::cout << "Total entities in storage: " << entityManager.getEntityCount() << std::endl;
    std::cout << std::endl;
    std::cout << "ECS system with SOA TransformStorage is working correctly!" << std::endl;

    return 0;
}
