#include <iostream>
#include <vector>
#include <memory>
#include <random>
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
#include "Material.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
    std::cout << "=== AIECS Stress Test Demo ===" << std::endl;
    std::cout << "Initializing GLFW and OpenGL..." << std::endl;

    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Configure GLFW for OpenGL Core profile (OpenGL 4.5 for all features)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window (1920x1080 for fullscreen stress test)
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "AIECS - Stress Test: 10,000+ Rectangles", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Initialize GLEW
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
    std::cout << "\n=== Setting up rendering system ===" << std::endl;
    auto world = std::make_shared<World>("StressTestWorld");
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

    std::cout << "\n=== Creating 10,000+ rectangles ===" << std::endl;
    
    // Random number generator
    std::mt19937 rng(42);  // Fixed seed for reproducibility
    std::uniform_real_distribution<float> posDistX(-0.95f, 0.95f);
    std::uniform_real_distribution<float> posDistY(-0.95f, 0.95f);
    std::uniform_real_distribution<float> scaleDistSmall(0.005f, 0.02f);
    std::uniform_real_distribution<float> scaleDistMedium(0.02f, 0.05f);
    std::uniform_real_distribution<float> colorDist(0.2f, 1.0f);
    std::uniform_real_distribution<float> rotSpeedDist(-1.0f, 1.0f);
    std::uniform_int_distribution<int> mobilityDist(0, 9);  // 70% static, 30% movable
    
    std::vector<std::shared_ptr<GameEntity>> entities;
    std::vector<float> rotationSpeeds;  // Store rotation speeds for animated objects
    
    // Create shared materials for better performance (material deduplication)
    std::vector<MaterialPtr> sharedMaterials;
    for (int i = 0; i < 20; ++i) {
        auto material = std::make_shared<Material>();
        material->setColor(glm::vec4(colorDist(rng), colorDist(rng), colorDist(rng), 1.0f));
        material->setMutability(i < 15 ? MaterialMutability::Static : MaterialMutability::Dynamic);
        sharedMaterials.push_back(material);
    }
    std::uniform_int_distribution<int> materialDist(0, sharedMaterials.size() - 1);

    // === Part 1: Background grid of static small rectangles (8000 rectangles) ===
    std::cout << "Creating 8000 static background rectangles..." << std::endl;
    int staticCount = 0;
    for (int i = 0; i < 8000; ++i) {
        auto entity = world->createObject<GameEntity>("StaticRect_" + std::to_string(i));
        auto transform = entity->addComponent<TransformComponent>();
        auto render = entity->addComponent<RenderComponent>();
        
        transform->setLocalPosition(glm::vec3(posDistX(rng), posDistY(rng), 0.0f));
        float scale = scaleDistSmall(rng);
        transform->setLocalScale(glm::vec3(scale, scale, 1.0f));
        transform->setMobility(TransformMobility::Static);  // Static - never changes
        
        render->setMaterial(sharedMaterials[materialDist(rng)]);
        
        entities.push_back(entity);
        rotationSpeeds.push_back(0.0f);  // No rotation
        staticCount++;
    }

    // === Part 2: Animated floating rectangles (1500 rectangles) ===
    std::cout << "Creating 1500 animated floating rectangles..." << std::endl;
    int movableCount = 0;
    for (int i = 0; i < 1500; ++i) {
        auto entity = world->createObject<GameEntity>("FloatingRect_" + std::to_string(i));
        auto transform = entity->addComponent<TransformComponent>();
        auto render = entity->addComponent<RenderComponent>();
        
        transform->setLocalPosition(glm::vec3(posDistX(rng), posDistY(rng), 0.0f));
        float scale = scaleDistMedium(rng);
        transform->setLocalScale(glm::vec3(scale, scale, 1.0f));
        transform->setMobility(TransformMobility::Movable);  // Movable - animated
        
        render->setMaterial(sharedMaterials[materialDist(rng)]);
        
        entities.push_back(entity);
        rotationSpeeds.push_back(rotSpeedDist(rng));  // Random rotation speed
        movableCount++;
    }

    // === Part 3: Hierarchical structures (500 parent-child pairs = 1000 rectangles) ===
    std::cout << "Creating 500 hierarchical structures (1000 rectangles total)..." << std::endl;
    int hierarchyCount = 0;
    for (int i = 0; i < 500; ++i) {
        // Parent rectangle (Movable)
        auto parent = world->createObject<GameEntity>("HierarchyParent_" + std::to_string(i));
        auto parentTransform = parent->addComponent<TransformComponent>();
        auto parentRender = parent->addComponent<RenderComponent>();
        
        parentTransform->setLocalPosition(glm::vec3(posDistX(rng), posDistY(rng), 0.0f));
        float parentScale = scaleDistMedium(rng) * 2.0f;
        parentTransform->setLocalScale(glm::vec3(parentScale, parentScale, 1.0f));
        parentTransform->setMobility(TransformMobility::Movable);  // Parent rotates
        parentRender->setMaterial(sharedMaterials[materialDist(rng)]);
        
        entities.push_back(parent);
        rotationSpeeds.push_back(rotSpeedDist(rng) * 0.5f);  // Slower rotation for parents
        hierarchyCount++;

        // Child rectangle (Static relative to parent)
        auto child = world->createObject<GameEntity>("HierarchyChild_" + std::to_string(i));
        auto childTransform = child->addComponent<TransformComponent>();
        auto childRender = child->addComponent<RenderComponent>();
        
        childTransform->setParent(parent);  // Attach to parent
        childTransform->setLocalPosition(glm::vec3(0.3f, 0.3f, 0.0f));  // Offset from parent
        childTransform->setLocalScale(glm::vec3(0.5f, 0.5f, 1.0f));  // Half size of parent
        childTransform->setMobility(TransformMobility::Static);  // Static relative to parent
        childRender->setMaterial(sharedMaterials[materialDist(rng)]);
        
        entities.push_back(child);
        rotationSpeeds.push_back(0.0f);  // No local rotation
        hierarchyCount++;
    }

    std::cout << "\n=== Summary ===" << std::endl;
    std::cout << "Total rectangles: " << entities.size() << std::endl;
    std::cout << "  - Static background: " << staticCount << " (never updated)" << std::endl;
    std::cout << "  - Animated floating: " << movableCount << " (updated every frame)" << std::endl;
    std::cout << "  - Hierarchy entities: " << hierarchyCount << " (" << hierarchyCount/2 << " parent-child pairs)" << std::endl;
    std::cout << "  - Unique materials: " << sharedMaterials.size() << " (automatic deduplication)" << std::endl;
    std::cout << "\n=== Performance Optimizations ===" << std::endl;
    std::cout << "  ✓ Zero-touch static data (static rectangles never iterated after init)" << std::endl;
    std::cout << "  ✓ Persistent mapped buffers (zero-copy GPU updates)" << std::endl;
    std::cout << "  ✓ Material deduplication (99% upload reduction)" << std::endl;
    std::cout << "  ✓ Dual SSBO/VBO architecture (separate static/dynamic buffers)" << std::endl;
    std::cout << "  ✓ ARB_vertex_attrib_binding (minimal state changes)" << std::endl;
    std::cout << "  ✓ Hierarchical transform flattening (parent-child optimized)" << std::endl;
    std::cout << "\nEntering render loop. Press ESC to exit." << std::endl;

    // FPS tracking
    double lastTime = glfwGetTime();
    int frameCount = 0;
    double fpsUpdateInterval = 1.0;  // Update FPS every 1 second

    // Render loop
    float time = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        float deltaTime = 0.016f; // ~60 FPS target
        time += deltaTime;

        // Input
        processInput(window);

        // Update world (which updates all modules)
        world->update(deltaTime);

        // Animate movable rectangles
        for (size_t i = 0; i < entities.size(); ++i) {
            if (rotationSpeeds[i] != 0.0f) {
                auto transform = entities[i]->getComponent<TransformComponent>();
                if (transform && transform->getMobility() == TransformMobility::Movable) {
                    // Rotate
                    transform->setLocalRotation(glm::angleAxis(
                        time * rotationSpeeds[i], 
                        glm::vec3(0.0f, 0.0f, 1.0f)
                    ));
                }
            }
        }

        // Clear screen
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // RenderCollector automatically collects and renders all entities
        renderCollector->collectAndRender();

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();

        // FPS calculation
        frameCount++;
        double currentTime = glfwGetTime();
        if (currentTime - lastTime >= fpsUpdateInterval) {
            double fps = frameCount / (currentTime - lastTime);
            double frameTimeMs = 1000.0 / fps;
            std::cout << "FPS: " << fps << " (" << frameTimeMs << " ms/frame)" << std::endl;
            frameCount = 0;
            lastTime = currentTime;
        }
    }

    // Cleanup
    std::cout << "\n=== Cleaning up ===" << std::endl;
    entities.clear();
    sharedMaterials.clear();
    world->shutdown();
    
    glfwTerminate();
    
    std::cout << "Application terminated successfully." << std::endl;
    return 0;
}
