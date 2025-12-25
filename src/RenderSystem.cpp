#include "RenderSystem.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// Vertex shader with SSBO-based rendering
// Uses material ID and matrix ID to lookup data from SSBOs
const char* vertexShaderSource = R"(
#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in uint aMaterialID;
layout (location = 2) in uint aMatrixID;

// SSBO for materials (colors)
layout (std430, binding = 0) buffer MaterialBuffer {
    vec4 materials[];
};

// SSBO for matrices
layout (std430, binding = 1) buffer MatrixBuffer {
    mat4 matrices[];
};

uniform mat4 projection;

out vec4 vColor;

void main()
{
    // Lookup matrix and material from SSBOs
    mat4 modelMatrix = matrices[aMatrixID];
    vec4 materialColor = materials[aMaterialID];
    
    gl_Position = projection * modelMatrix * vec4(aPos, 1.0);
    vColor = materialColor;
}
)";

// Fragment shader - unchanged
const char* fragmentShaderSource = R"(
#version 430 core
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
        if (instanceDataVBO != 0) {
            glDeleteBuffers(1, &instanceDataVBO);
            instanceDataVBO = 0;
        }
        if (materialSSBO != 0) {
            glDeleteBuffers(1, &materialSSBO);
            materialSSBO = 0;
        }
        if (matrixSSBO != 0) {
            glDeleteBuffers(1, &matrixSSBO);
            matrixSSBO = 0;
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

    std::cout << "[RenderSystem] Initializing OpenGL resources with SSBO-based rendering..." << std::endl;

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
    glGenBuffers(1, &instanceDataVBO);
    glGenBuffers(1, &materialSSBO);
    glGenBuffers(1, &matrixSSBO);

    glBindVertexArray(VAO);

    // Setup vertex buffer (position data)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Setup instance data buffer (materialID and matrixID per instance)
    // Each instance has 2 uints: materialID, matrixID
    glBindBuffer(GL_ARRAY_BUFFER, instanceDataVBO);
    glBufferData(GL_ARRAY_BUFFER, ssboCapacity * 2 * sizeof(unsigned int), nullptr, GL_DYNAMIC_DRAW);
    
    // Material ID (location 1)
    glEnableVertexAttribArray(1);
    glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, 2 * sizeof(unsigned int), (void*)0);
    glVertexAttribDivisor(1, 1); // Per instance
    
    // Matrix ID (location 2)
    glEnableVertexAttribArray(2);
    glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, 2 * sizeof(unsigned int), (void*)sizeof(unsigned int));
    glVertexAttribDivisor(2, 1); // Per instance

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Setup Material SSBO (binding point 0)
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, materialSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, ssboCapacity * sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, materialSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // Setup Matrix SSBO (binding point 1)
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, matrixSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, ssboCapacity * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, matrixSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // Set up orthographic projection for 2D rendering
    projectionMatrix = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);
    
    glUseProgram(shaderProgram);
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    glInitialized = true;
    std::cout << "[RenderSystem] SSBO-based rendering initialization complete." << std::endl;
}

void RenderSystem::renderBatch(const std::vector<glm::mat4>& modelMatrices, 
                                const std::vector<glm::vec4>& colors) {
    if (!glInitialized) return;
    if (modelMatrices.empty() || modelMatrices.size() != colors.size()) return;

    size_t instanceCount = modelMatrices.size();
    
    // Resize SSBOs if needed
    if (instanceCount > ssboCapacity) {
        ssboCapacity = instanceCount * 2; // Double the capacity
        
        // Resize material SSBO
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, materialSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, ssboCapacity * sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, materialSSBO);
        
        // Resize matrix SSBO
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, matrixSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, ssboCapacity * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, matrixSSBO);
        
        // Resize instance data VBO
        glBindBuffer(GL_ARRAY_BUFFER, instanceDataVBO);
        glBufferData(GL_ARRAY_BUFFER, ssboCapacity * 2 * sizeof(unsigned int), nullptr, GL_DYNAMIC_DRAW);
        
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        std::cout << "[RenderSystem] Resized SSBOs to " << ssboCapacity << std::endl;
    }

    // Upload materials to SSBO
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, materialSSBO);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, instanceCount * sizeof(glm::vec4), colors.data());

    // Upload matrices to SSBO
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, matrixSSBO);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, instanceCount * sizeof(glm::mat4), modelMatrices.data());

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // Build instance data (materialID and matrixID for each instance)
    std::vector<unsigned int> instanceData;
    instanceData.reserve(instanceCount * 2);
    for (size_t i = 0; i < instanceCount; ++i) {
        instanceData.push_back(static_cast<unsigned int>(i)); // materialID
        instanceData.push_back(static_cast<unsigned int>(i)); // matrixID
    }

    // Upload instance data
    glBindBuffer(GL_ARRAY_BUFFER, instanceDataVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, instanceData.size() * sizeof(unsigned int), instanceData.data());

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
