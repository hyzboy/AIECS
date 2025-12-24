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

    // Demonstrate parent-child relationships
    std::cout << "=== Testing Parent-Child Relationships ===" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Setting entity2 and entity3 as children of entity1..." << std::endl;
    entity1.addChild(entity2);
    entity1.addChild(entity3);
    
    std::cout << "Entity 1 has " << entity1.getChildCount() << " children" << std::endl;
    std::cout << "Entity 2 has parent: " << (entity2.hasParent() ? "Yes" : "No") << std::endl;
    std::cout << "Entity 3 has parent: " << (entity3.hasParent() ? "Yes" : "No") << std::endl;
    
    // Display children
    auto children = entity1.getChildren();
    std::cout << "Entity 1's children: ";
    for (const auto& child : children) {
        std::cout << "ID " << child.getID() << " ";
    }
    std::cout << std::endl;
    
    // Check parent
    Entity parent2 = entity2.getParent();
    if (parent2.isValid()) {
        std::cout << "Entity 2's parent ID: " << parent2.getID() << std::endl;
    }
    std::cout << std::endl;

    // Create more entities for a deeper hierarchy
    Entity entity4 = entityManager.createEntity();
    Entity entity5 = entityManager.createEntity();
    
    entity4.setPosition(glm::vec3(1.0f, 0.0f, 0.0f));
    entity5.setPosition(glm::vec3(0.0f, 1.0f, 0.0f));
    
    std::cout << "Creating deeper hierarchy..." << std::endl;
    entity2.addChild(entity4);
    entity2.addChild(entity5);
    
    std::cout << "Entity 2 now has " << entity2.getChildCount() << " children" << std::endl;
    std::cout << "Entity 4's parent ID: " << entity4.getParent().getID() << std::endl;
    std::cout << std::endl;

    // Test removing a child
    std::cout << "Removing entity3 from entity1's children..." << std::endl;
    entity1.removeChild(entity3);
    std::cout << "Entity 1 now has " << entity1.getChildCount() << " children" << std::endl;
    std::cout << "Entity 3 has parent: " << (entity3.hasParent() ? "Yes" : "No") << std::endl;
    std::cout << std::endl;

    // Demonstrate transform deletion with children
    std::cout << "Deleting entity2's transform (which has children)..." << std::endl;
    size_t beforeCount = entity2.getChildCount();
    std::cout << "Entity 2 had " << beforeCount << " children before deletion" << std::endl;
    entity2.deleteTransform();
    std::cout << "Entity 2 valid: " << (entity2.isValid() ? "Yes" : "No") << std::endl;
    std::cout << "Entity 4 still valid: " << (entity4.isValid() ? "Yes" : "No") << std::endl;
    std::cout << "Entity 4 has parent after entity2 deletion: " << (entity4.hasParent() ? "Yes" : "No") << std::endl;
    std::cout << std::endl;

    std::cout << "Total entities in storage: " << entityManager.getEntityCount() << std::endl;
    std::cout << std::endl;

    // Demonstrate World Transform Matrix calculation
    std::cout << "=== Testing World Transform Matrix ===" << std::endl;
    std::cout << std::endl;

    // Create a new hierarchy for testing world transforms
    Entity parent = entityManager.createEntity();
    Entity child = entityManager.createEntity();
    Entity grandchild = entityManager.createEntity();

    // Set up parent transform
    parent.setPosition(glm::vec3(10.0f, 0.0f, 0.0f));
    parent.setScale(glm::vec3(2.0f, 2.0f, 2.0f));
    
    // Set up child transform (relative to parent)
    child.setPosition(glm::vec3(5.0f, 0.0f, 0.0f));
    
    // Set up grandchild transform (relative to child)
    grandchild.setPosition(glm::vec3(3.0f, 0.0f, 0.0f));

    // Build hierarchy
    parent.addChild(child);
    child.addChild(grandchild);

    std::cout << "Created hierarchy: parent -> child -> grandchild" << std::endl;
    std::cout << "Parent position: (10, 0, 0), scale: (2, 2, 2)" << std::endl;
    std::cout << "Child position (local): (5, 0, 0)" << std::endl;
    std::cout << "Grandchild position (local): (3, 0, 0)" << std::endl;
    std::cout << std::endl;

    // Update transforms
    std::cout << "Updating world transforms..." << std::endl;
    parent.updateTransformHierarchy();
    
    // Display world matrices
    glm::mat4 parentWorld = parent.getWorldMatrix();
    glm::mat4 childWorld = child.getWorldMatrix();
    glm::mat4 grandchildWorld = grandchild.getWorldMatrix();
    
    std::cout << "Parent world position: (" 
              << parentWorld[3][0] << ", " 
              << parentWorld[3][1] << ", " 
              << parentWorld[3][2] << ")" << std::endl;
    
    std::cout << "Child world position: (" 
              << childWorld[3][0] << ", " 
              << childWorld[3][1] << ", " 
              << childWorld[3][2] << ")" << std::endl;
    
    std::cout << "Grandchild world position: (" 
              << grandchildWorld[3][0] << ", " 
              << grandchildWorld[3][1] << ", " 
              << grandchildWorld[3][2] << ")" << std::endl;
    std::cout << std::endl;

    // Test updating all transforms via EntityManager
    std::cout << "Testing EntityManager.updateAllTransforms()..." << std::endl;
    parent.setPosition(glm::vec3(20.0f, 5.0f, 0.0f));
    entityManager.updateAllTransforms();
    
    parentWorld = parent.getWorldMatrix();
    childWorld = child.getWorldMatrix();
    
    std::cout << "After moving parent to (20, 5, 0):" << std::endl;
    std::cout << "Parent world position: (" 
              << parentWorld[3][0] << ", " 
              << parentWorld[3][1] << ", " 
              << parentWorld[3][2] << ")" << std::endl;
    
    std::cout << "Child world position: (" 
              << childWorld[3][0] << ", " 
              << childWorld[3][1] << ", " 
              << childWorld[3][2] << ")" << std::endl;
    std::cout << std::endl;

    std::cout << "ECS system with hierarchical world transform matrices is working correctly!" << std::endl;

    return 0;
}
