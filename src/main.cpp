#include <iostream>
#include <vector>
#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "World.h"
#include "GameEntity.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "RenderSystem.h"
#include "RenderCollector.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
    std::cout << "Initializing GLFW and OpenGL..." << std::endl;

    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Configure GLFW for OpenGL Core profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow* window = glfwCreateWindow(800, 600, "AIECS - Modular Rendering System", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Initialize GLEW (glewExperimental needed for Core profile to load all extensions)
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    // Create world and modules
    std::cout << "\nCreating world and modules..." << std::endl;
    auto world = std::make_shared<World>("MainWorld");
    world->initialize();

    // Register RenderSystem module
    auto renderSystem = world->registerModule<RenderSystem>();
    renderSystem->initialize();
    renderSystem->initializeGL();

    // Register RenderCollector module
    auto renderCollector = world->registerModule<RenderCollector>();
    renderCollector->initialize();
    renderCollector->setWorld(world);
    renderCollector->setRenderSystem(renderSystem);

    std::cout << "\nCreating hierarchical entity structure..." << std::endl;
    
    // Create hierarchical structure: Large parent rectangle with smaller children
    std::vector<std::shared_ptr<GameEntity>> entities;

    // Parent Rectangle: Large blue rectangle at center
    std::shared_ptr<GameEntity> parentRect;
    {
        auto entity = world->createObject<GameEntity>("ParentRectangle");
        auto transform = entity->addComponent<TransformComponent>();
        auto render = entity->addComponent<RenderComponent>();
        
        transform->setLocalPosition(glm::vec3(0.0f, 0.0f, 0.0f));
        transform->setLocalScale(glm::vec3(1.5f, 1.5f, 1.0f)); // Large parent
        transform->setMobility(TransformMobility::Movable);  // Parent is movable (animated)
        render->setColor(glm::vec4(0.2f, 0.4f, 0.6f, 1.0f)); // Dark blue
        
        entities.push_back(entity);
        parentRect = entity;
    }

    // Child Rectangle 1: Small red rectangle (top-left of parent) - Static
    {
        auto entity = world->createObject<GameEntity>("ChildRect1");
        auto transform = entity->addComponent<TransformComponent>();
        auto render = entity->addComponent<RenderComponent>();
        
        transform->setParent(parentRect);
        transform->setLocalPosition(glm::vec3(-0.3f, 0.3f, 0.0f));
        transform->setLocalScale(glm::vec3(0.3f, 0.3f, 1.0f));
        transform->setMobility(TransformMobility::Static);  // Never moves relative to parent
        render->setColor(glm::vec4(0.9f, 0.2f, 0.2f, 1.0f)); // Red
        
        entities.push_back(entity);
    }

    // Child Rectangle 2: Small green rectangle (top-right of parent) - Static
    {
        auto entity = world->createObject<GameEntity>("ChildRect2");
        auto transform = entity->addComponent<TransformComponent>();
        auto render = entity->addComponent<RenderComponent>();
        
        transform->setParent(parentRect);
        transform->setLocalPosition(glm::vec3(0.3f, 0.3f, 0.0f));
        transform->setLocalScale(glm::vec3(0.3f, 0.3f, 1.0f));
        transform->setMobility(TransformMobility::Static);  // Never moves relative to parent
        render->setColor(glm::vec4(0.2f, 0.9f, 0.2f, 1.0f)); // Green
        
        entities.push_back(entity);
    }

    // Child Rectangle 3: Small yellow rectangle (bottom-left of parent) - Static
    {
        auto entity = world->createObject<GameEntity>("ChildRect3");
        auto transform = entity->addComponent<TransformComponent>();
        auto render = entity->addComponent<RenderComponent>();
        
        transform->setParent(parentRect);
        transform->setLocalPosition(glm::vec3(-0.3f, -0.3f, 0.0f));
        transform->setLocalScale(glm::vec3(0.3f, 0.3f, 1.0f));
        transform->setMobility(TransformMobility::Static);  // Static object
        render->setColor(glm::vec4(0.9f, 0.9f, 0.2f, 1.0f)); // Yellow
        
        entities.push_back(entity);
    }

    // Child Rectangle 4: Small magenta rectangle (bottom-right of parent) - Static
    {
        auto entity = world->createObject<GameEntity>("ChildRect4");
        auto transform = entity->addComponent<TransformComponent>();
        auto render = entity->addComponent<RenderComponent>();
        
        transform->setParent(parentRect);
        transform->setLocalPosition(glm::vec3(0.3f, -0.3f, 0.0f));
        transform->setLocalScale(glm::vec3(0.3f, 0.3f, 1.0f));
        transform->setMobility(TransformMobility::Static);  // Static object
        render->setColor(glm::vec4(0.9f, 0.2f, 0.9f, 1.0f)); // Magenta
        
        entities.push_back(entity);
    }

    // Child Rectangle 5: Tiny white rectangle at center of parent - Movable
    {
        auto entity = world->createObject<GameEntity>("ChildRect5");
        auto transform = entity->addComponent<TransformComponent>();
        auto render = entity->addComponent<RenderComponent>();
        
        transform->setParent(parentRect);
        transform->setLocalPosition(glm::vec3(0.0f, 0.0f, 0.0f));
        transform->setLocalScale(glm::vec3(0.2f, 0.2f, 1.0f));
        transform->setMobility(TransformMobility::Movable);  // Movable (will animate)
        render->setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)); // White
        
        entities.push_back(entity);
    }

    std::cout << "Created " << entities.size() << " rectangles in hierarchical structure." << std::endl;
    std::cout << "  1 parent (Movable, animated)" << std::endl;
    std::cout << "  4 Static children (red, green, yellow, magenta) - matrix cached" << std::endl;
    std::cout << "  1 Movable child (white, rotating)" << std::endl;
    std::cout << "\n[Architecture] Using modular system:" << std::endl;
    std::cout << "  - RenderSystem: Handles all OpenGL rendering with instancing" << std::endl;
    std::cout << "  - RenderCollector: Collects component data and batches rendering" << std::endl;
    std::cout << "\n[Optimization] Transform mobility:" << std::endl;
    std::cout << "  - Static: Matrix cached, never recalculated" << std::endl;
    std::cout << "  - Movable: Matrix recalculated every frame" << std::endl;
    std::cout << "\nEntering render loop. Press ESC to exit." << std::endl;

    // Render loop
    float time = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        float deltaTime = 0.016f; // ~60 FPS
        time += deltaTime;

        // Input
        processInput(window);

        // Update world (which updates all modules)
        world->update(deltaTime);

        // Animate the parent rectangle (rotation and slight scaling)
        // Parent is Movable, so it will be updated every frame
        auto parentTransform = parentRect->getComponent<TransformComponent>();
        if (parentTransform) {
            // Rotate parent slowly
            parentTransform->setLocalRotation(glm::angleAxis(time * 0.5f, glm::vec3(0.0f, 0.0f, 1.0f)));
            
            // Pulse scale slightly (1.3 to 1.7)
            float scale = 1.5f + 0.2f * sin(time * 2.0f);
            parentTransform->setLocalScale(glm::vec3(scale, scale, 1.0f));
        }

        // Animate the white child (Movable) - local rotation
        if (entities.size() > 5) {
            auto whiteTransform = entities[5]->getComponent<TransformComponent>();
            if (whiteTransform) {
                whiteTransform->setLocalRotation(glm::angleAxis(time * -2.0f, glm::vec3(0.0f, 0.0f, 1.0f)));
            }
        }

        // Clear screen
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // RenderCollector automatically collects and renders all entities
        renderCollector->collectAndRender();

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    std::cout << "\nCleaning up..." << std::endl;
    entities.clear();
    world->shutdown();
    
    glfwTerminate();
    
    std::cout << "Application terminated successfully." << std::endl;
    return 0;
}
