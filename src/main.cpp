#include <iostream>
#include <vector>
#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "World.h"
#include "GameEntity.h"
#include "TransformComponent.h"
#include "RenderComponent.h"

// Vertex shader with model matrix for transformations
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 projection;

void main()
{
    gl_Position = projection * model * vec4(aPos, 1.0);
}
)";

// Fragment shader with color uniform
const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

uniform vec4 rectColor;

void main()
{
    FragColor = rectColor;
}
)";

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

unsigned int compileShader(GLenum type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return shader;
}

unsigned int createShaderProgram() {
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return shaderProgram;
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
    GLFWwindow* window = glfwCreateWindow(800, 600, "AIECS - 2D Rectangle Rendering", nullptr, nullptr);
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

    // Create shader program
    unsigned int shaderProgram = createShaderProgram();

    // Create orthographic projection matrix (for 2D rendering)
    glm::mat4 projection = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);
    
    glUseProgram(shaderProgram);
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Create world and entities
    std::cout << "Creating world and entities..." << std::endl;
    auto world = std::make_shared<World>("MainWorld");
    world->initialize();

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
        render->setColor(glm::vec4(0.2f, 0.4f, 0.6f, 1.0f)); // Dark blue
        render->initializeGL();
        
        entities.push_back(entity);
        parentRect = entity;
    }

    // Child Rectangle 1: Small red rectangle (top-left of parent)
    {
        auto entity = world->createObject<GameEntity>("ChildRect1");
        auto transform = entity->addComponent<TransformComponent>();
        auto render = entity->addComponent<RenderComponent>();
        
        // Set parent relationship
        transform->setParent(parentRect);
        
        // Position relative to parent (local coordinates)
        transform->setLocalPosition(glm::vec3(-0.3f, 0.3f, 0.0f));
        transform->setLocalScale(glm::vec3(0.3f, 0.3f, 1.0f)); // Small
        render->setColor(glm::vec4(0.9f, 0.2f, 0.2f, 1.0f)); // Red
        render->initializeGL();
        
        entities.push_back(entity);
    }

    // Child Rectangle 2: Small green rectangle (top-right of parent)
    {
        auto entity = world->createObject<GameEntity>("ChildRect2");
        auto transform = entity->addComponent<TransformComponent>();
        auto render = entity->addComponent<RenderComponent>();
        
        transform->setParent(parentRect);
        
        transform->setLocalPosition(glm::vec3(0.3f, 0.3f, 0.0f));
        transform->setLocalScale(glm::vec3(0.3f, 0.3f, 1.0f));
        render->setColor(glm::vec4(0.2f, 0.9f, 0.2f, 1.0f)); // Green
        render->initializeGL();
        
        entities.push_back(entity);
    }

    // Child Rectangle 3: Small yellow rectangle (bottom-left of parent)
    {
        auto entity = world->createObject<GameEntity>("ChildRect3");
        auto transform = entity->addComponent<TransformComponent>();
        auto render = entity->addComponent<RenderComponent>();
        
        transform->setParent(parentRect);
        
        transform->setLocalPosition(glm::vec3(-0.3f, -0.3f, 0.0f));
        transform->setLocalScale(glm::vec3(0.3f, 0.3f, 1.0f));
        render->setColor(glm::vec4(0.9f, 0.9f, 0.2f, 1.0f)); // Yellow
        render->initializeGL();
        
        entities.push_back(entity);
    }

    // Child Rectangle 4: Small magenta rectangle (bottom-right of parent)
    {
        auto entity = world->createObject<GameEntity>("ChildRect4");
        auto transform = entity->addComponent<TransformComponent>();
        auto render = entity->addComponent<RenderComponent>();
        
        transform->setParent(parentRect);
        
        transform->setLocalPosition(glm::vec3(0.3f, -0.3f, 0.0f));
        transform->setLocalScale(glm::vec3(0.3f, 0.3f, 1.0f));
        render->setColor(glm::vec4(0.9f, 0.2f, 0.9f, 1.0f)); // Magenta
        render->initializeGL();
        
        entities.push_back(entity);
    }

    // Child Rectangle 5: Tiny white rectangle at center of parent
    {
        auto entity = world->createObject<GameEntity>("ChildRect5");
        auto transform = entity->addComponent<TransformComponent>();
        auto render = entity->addComponent<RenderComponent>();
        
        transform->setParent(parentRect);
        
        transform->setLocalPosition(glm::vec3(0.0f, 0.0f, 0.0f));
        transform->setLocalScale(glm::vec3(0.2f, 0.2f, 1.0f)); // Tiny
        render->setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)); // White
        render->initializeGL();
        
        entities.push_back(entity);
    }

    std::cout << "Created " << entities.size() << " rectangles in hierarchical structure." << std::endl;
    std::cout << "  1 parent (large blue) + 5 children (red, green, yellow, magenta, white)" << std::endl;
    std::cout << "Entering render loop. Press ESC to exit." << std::endl;

    // Render loop
    float time = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = 0.016f; // ~60 FPS
        time += deltaTime;

        // Input
        processInput(window);

        // Update world
        world->update(deltaTime);

        // Animate the parent rectangle (rotation and slight scaling)
        // This should affect all child rectangles due to the transform hierarchy
        auto parentTransform = parentRect->getComponent<TransformComponent>();
        if (parentTransform) {
            // Rotate parent slowly
            parentTransform->setLocalRotation(glm::angleAxis(time * 0.5f, glm::vec3(0.0f, 0.0f, 1.0f)));
            
            // Pulse scale slightly (1.3 to 1.7)
            float scale = 1.5f + 0.2f * sin(time * 2.0f);
            parentTransform->setLocalScale(glm::vec3(scale, scale, 1.0f));
        }

        // Render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // Render all entities
        for (auto& entity : entities) {
            auto render = entity->getComponent<RenderComponent>();
            if (render) {
                render->render(shaderProgram, entity.get());
            }
        }

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    std::cout << "Cleaning up..." << std::endl;
    entities.clear();
    world->shutdown();
    
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    
    std::cout << "Application terminated successfully." << std::endl;
    return 0;
}
