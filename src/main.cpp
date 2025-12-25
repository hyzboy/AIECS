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

    // Create multiple rectangles with different transforms and colors
    std::vector<std::shared_ptr<GameEntity>> entities;

    // Rectangle 1: Center, blue
    {
        auto entity = world->createObject<GameEntity>("Rectangle1");
        auto transform = entity->addComponent<TransformComponent>();
        auto render = entity->addComponent<RenderComponent>();
        
        transform->setLocalPosition(glm::vec3(0.0f, 0.0f, 0.0f));
        transform->setLocalScale(glm::vec3(1.0f, 1.0f, 1.0f));
        render->setColor(glm::vec4(0.2f, 0.6f, 0.8f, 1.0f)); // Blue
        render->initializeGL();
        
        entities.push_back(entity);
    }

    // Rectangle 2: Left, green, smaller
    {
        auto entity = world->createObject<GameEntity>("Rectangle2");
        auto transform = entity->addComponent<TransformComponent>();
        auto render = entity->addComponent<RenderComponent>();
        
        transform->setLocalPosition(glm::vec3(-1.2f, 0.0f, 0.0f));
        transform->setLocalScale(glm::vec3(0.6f, 0.6f, 1.0f));
        render->setColor(glm::vec4(0.2f, 0.8f, 0.2f, 1.0f)); // Green
        render->initializeGL();
        
        entities.push_back(entity);
    }

    // Rectangle 3: Right, red, rotated
    {
        auto entity = world->createObject<GameEntity>("Rectangle3");
        auto transform = entity->addComponent<TransformComponent>();
        auto render = entity->addComponent<RenderComponent>();
        
        transform->setLocalPosition(glm::vec3(1.2f, 0.0f, 0.0f));
        transform->setLocalScale(glm::vec3(0.8f, 0.8f, 1.0f));
        transform->setLocalRotation(glm::angleAxis(glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        render->setColor(glm::vec4(0.8f, 0.2f, 0.2f, 1.0f)); // Red
        render->initializeGL();
        
        entities.push_back(entity);
    }

    std::cout << "Created " << entities.size() << " rectangles." << std::endl;
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

        // Animate the center rectangle rotation
        auto transform1 = entities[0]->getComponent<TransformComponent>();
        if (transform1) {
            transform1->setLocalRotation(glm::angleAxis(time, glm::vec3(0.0f, 0.0f, 1.0f)));
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
