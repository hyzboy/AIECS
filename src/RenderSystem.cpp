#include "RenderSystem.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// Vertex shader with instanced rendering support
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in mat4 aInstanceMatrix;
layout (location = 5) in vec4 aInstanceColor;

uniform mat4 projection;

out vec4 vColor;

void main()
{
    gl_Position = projection * aInstanceMatrix * vec4(aPos, 1.0);
    vColor = aInstanceColor;
}
)";

// Fragment shader with per-instance color
const char* fragmentShaderSource = R"(
#version 330 core
in vec4 vColor;
out vec4 FragColor;

void main()
{
    FragColor = vColor;
}
)";

RenderSystem::RenderSystem(const std::string& name)
    : Module(name) {
}

RenderSystem::~RenderSystem() {
    shutdown();
}

void RenderSystem::initialize() {
    std::cout << "[RenderSystem] Initializing..." << std::endl;
}

void RenderSystem::update(float deltaTime) {
    // RenderSystem doesn't need per-frame updates
}

void RenderSystem::shutdown() {
    if (glInitialized) {
        if (VAO != 0) {
            glDeleteVertexArrays(1, &VAO);
            VAO = 0;
        }
        if (VBO != 0) {
            glDeleteBuffers(1, &VBO);
            VBO = 0;
        }
        if (instanceMatrixVBO != 0) {
            glDeleteBuffers(1, &instanceMatrixVBO);
            instanceMatrixVBO = 0;
        }
        if (instanceColorVBO != 0) {
            glDeleteBuffers(1, &instanceColorVBO);
            instanceColorVBO = 0;
        }
        if (shaderProgram != 0) {
            glDeleteProgram(shaderProgram);
            shaderProgram = 0;
        }
        glInitialized = false;
    }
    std::cout << "[RenderSystem] Shutdown complete." << std::endl;
}

void RenderSystem::initializeGL() {
    if (glInitialized) return;

    std::cout << "[RenderSystem] Initializing OpenGL resources for instanced rendering..." << std::endl;

    // Create shader program
    shaderProgram = createShaderProgram();

    // Rectangle vertices from -0.5,-0.5 to +0.5,+0.5 (two triangles)
    float vertices[] = {
        // First triangle
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f,  0.5f, 0.0f,  // top left
        // Second triangle
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &instanceMatrixVBO);
    glGenBuffers(1, &instanceColorVBO);

    glBindVertexArray(VAO);

    // Setup vertex buffer (position data)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Setup instance matrix buffer (location 1-4, mat4 takes 4 attribute slots)
    glBindBuffer(GL_ARRAY_BUFFER, instanceMatrixVBO);
    glBufferData(GL_ARRAY_BUFFER, instanceBufferCapacity * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
    
    // mat4 attributes (location 1-4)
    for (unsigned int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(1 + i);
        glVertexAttribPointer(1 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), 
                             (void*)(sizeof(glm::vec4) * i));
        glVertexAttribDivisor(1 + i, 1); // Per instance, not per vertex
    }

    // Setup instance color buffer (location 5)
    glBindBuffer(GL_ARRAY_BUFFER, instanceColorVBO);
    glBufferData(GL_ARRAY_BUFFER, instanceBufferCapacity * sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
    glVertexAttribDivisor(5, 1); // Per instance

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Set up orthographic projection for 2D rendering
    projectionMatrix = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);
    
    glUseProgram(shaderProgram);
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    glInitialized = true;
    std::cout << "[RenderSystem] Instanced rendering initialization complete." << std::endl;
}

void RenderSystem::renderBatch(const std::vector<glm::mat4>& modelMatrices, 
                                const std::vector<glm::vec4>& colors) {
    if (!glInitialized) return;
    if (modelMatrices.empty() || modelMatrices.size() != colors.size()) return;

    size_t instanceCount = modelMatrices.size();
    
    // Resize buffers if needed
    if (instanceCount > instanceBufferCapacity) {
        instanceBufferCapacity = instanceCount * 2; // Double the capacity
        
        glBindBuffer(GL_ARRAY_BUFFER, instanceMatrixVBO);
        glBufferData(GL_ARRAY_BUFFER, instanceBufferCapacity * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
        
        glBindBuffer(GL_ARRAY_BUFFER, instanceColorVBO);
        glBufferData(GL_ARRAY_BUFFER, instanceBufferCapacity * sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
        
        std::cout << "[RenderSystem] Resized instance buffers to " << instanceBufferCapacity << std::endl;
    }

    // Upload instance data
    glBindBuffer(GL_ARRAY_BUFFER, instanceMatrixVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, instanceCount * sizeof(glm::mat4), modelMatrices.data());

    glBindBuffer(GL_ARRAY_BUFFER, instanceColorVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, instanceCount * sizeof(glm::vec4), colors.data());

    // Draw all instances in a single call
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, static_cast<GLsizei>(instanceCount));
    glBindVertexArray(0);
}

unsigned int RenderSystem::compileShader(GLenum type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "[RenderSystem] ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return shader;
}

unsigned int RenderSystem::createShaderProgram() {
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    
    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    
    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "[RenderSystem] ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return program;
}
