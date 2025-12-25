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
        
        // Clean up static resources
        if (staticMaterialIDVBO != 0) {
            glDeleteBuffers(1, &staticMaterialIDVBO);
            staticMaterialIDVBO = 0;
        }
        if (staticMatrixIDVBO != 0) {
            glDeleteBuffers(1, &staticMatrixIDVBO);
            staticMatrixIDVBO = 0;
        }
        if (staticMaterialSSBO != 0) {
            glDeleteBuffers(1, &staticMaterialSSBO);
            staticMaterialSSBO = 0;
        }
        if (staticMatrixSSBO != 0) {
            glDeleteBuffers(1, &staticMatrixSSBO);
            staticMatrixSSBO = 0;
        }
        
        // Clean up dynamic resources
        if (dynamicMaterialIDVBO != 0) {
            glDeleteBuffers(1, &dynamicMaterialIDVBO);
            dynamicMaterialIDVBO = 0;
        }
        if (dynamicMatrixIDVBO != 0) {
            glDeleteBuffers(1, &dynamicMatrixIDVBO);
            dynamicMatrixIDVBO = 0;
        }
        if (dynamicMaterialSSBO != 0) {
            glDeleteBuffers(1, &dynamicMaterialSSBO);
            dynamicMaterialSSBO = 0;
        }
        if (dynamicMatrixSSBO != 0) {
            glDeleteBuffers(1, &dynamicMatrixSSBO);
            dynamicMatrixSSBO = 0;
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

    std::cout << "[RenderSystem] Initializing dual SSBO/VBO architecture (static/dynamic separation)..." << std::endl;

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
    
    // Generate static buffers
    glGenBuffers(1, &staticMaterialIDVBO);
    glGenBuffers(1, &staticMatrixIDVBO);
    glGenBuffers(1, &staticMaterialSSBO);
    glGenBuffers(1, &staticMatrixSSBO);
    
    // Generate dynamic buffers
    glGenBuffers(1, &dynamicMaterialIDVBO);
    glGenBuffers(1, &dynamicMatrixIDVBO);
    glGenBuffers(1, &dynamicMaterialSSBO);
    glGenBuffers(1, &dynamicMatrixSSBO);

    glBindVertexArray(VAO);

    // Setup vertex buffer (position data) - shared between static and dynamic
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Note: We'll bind the appropriate VBOs (static or dynamic) before each draw call
    // Initialize them here but don't bind to VAO attributes yet
    
    // Setup static material ID buffer (location 1) - GL_STATIC_DRAW
    glBindBuffer(GL_ARRAY_BUFFER, staticMaterialIDVBO);
    glBufferData(GL_ARRAY_BUFFER, staticCapacity * sizeof(unsigned int), nullptr, GL_STATIC_DRAW);
    
    // Setup static matrix ID buffer (location 2) - GL_STATIC_DRAW
    glBindBuffer(GL_ARRAY_BUFFER, staticMatrixIDVBO);
    glBufferData(GL_ARRAY_BUFFER, staticCapacity * sizeof(unsigned int), nullptr, GL_STATIC_DRAW);
    
    // Setup dynamic material ID buffer (location 1) - GL_DYNAMIC_DRAW
    glBindBuffer(GL_ARRAY_BUFFER, dynamicMaterialIDVBO);
    glBufferData(GL_ARRAY_BUFFER, dynamicCapacity * sizeof(unsigned int), nullptr, GL_DYNAMIC_DRAW);
    
    // Setup dynamic matrix ID buffer (location 2) - GL_DYNAMIC_DRAW
    glBindBuffer(GL_ARRAY_BUFFER, dynamicMatrixIDVBO);
    glBufferData(GL_ARRAY_BUFFER, dynamicCapacity * sizeof(unsigned int), nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Setup Static Material SSBO (binding point 0) - GL_STATIC_DRAW
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, staticMaterialSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, staticCapacity * sizeof(glm::vec4), nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, staticMaterialSSBO);

    // Setup Static Matrix SSBO (binding point 1) - GL_STATIC_DRAW
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, staticMatrixSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, staticCapacity * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, staticMatrixSSBO);
    
    // Setup Dynamic Material SSBO (binding point 0) - GL_DYNAMIC_DRAW
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, dynamicMaterialSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, dynamicCapacity * sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, dynamicMaterialSSBO);

    // Setup Dynamic Matrix SSBO (binding point 1) - GL_DYNAMIC_DRAW
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, dynamicMatrixSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, dynamicCapacity * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, dynamicMatrixSSBO);
    
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // Set up orthographic projection for 2D rendering
    projectionMatrix = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);
    
    glUseProgram(shaderProgram);
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    glInitialized = true;
    std::cout << "[RenderSystem] Dual SSBO/VBO architecture initialization complete." << std::endl;
}

void RenderSystem::renderBatch(const std::vector<glm::mat4>& staticMatrices,
                                const std::vector<glm::vec4>& staticMaterials,
                                const std::vector<unsigned int>& staticMaterialIDs,
                                const std::vector<glm::mat4>& dynamicMatrices,
                                const std::vector<glm::vec4>& dynamicMaterials,
                                const std::vector<unsigned int>& dynamicMaterialIDs) {
    if (!glInitialized) return;
    
    size_t staticCount = staticMatrices.size();
    size_t dynamicCount = dynamicMatrices.size();
    
    if (staticCount != staticMaterialIDs.size() || dynamicCount != dynamicMaterialIDs.size()) {
        std::cerr << "[RenderSystem] Error: Size mismatch in render batch data" << std::endl;
        return;
    }
    
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    
    // === RENDER STATIC OBJECTS ===
    if (staticCount > 0 && !staticMaterials.empty()) {
        // Resize static buffers if needed (rare - only on first frame or when static objects added)
        size_t requiredStaticCapacity = std::max(staticCount, staticMaterials.size());
        if (requiredStaticCapacity > staticCapacity) {
            staticCapacity = requiredStaticCapacity * 2;
            
            // Resize static material SSBO
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, staticMaterialSSBO);
            glBufferData(GL_SHADER_STORAGE_BUFFER, staticCapacity * sizeof(glm::vec4), nullptr, GL_STATIC_DRAW);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, staticMaterialSSBO);
            
            // Resize static matrix SSBO
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, staticMatrixSSBO);
            glBufferData(GL_SHADER_STORAGE_BUFFER, staticCapacity * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, staticMatrixSSBO);
            
            // Resize static material ID VBO
            glBindBuffer(GL_ARRAY_BUFFER, staticMaterialIDVBO);
            glBufferData(GL_ARRAY_BUFFER, staticCapacity * sizeof(unsigned int), nullptr, GL_STATIC_DRAW);
            
            // Resize static matrix ID VBO
            glBindBuffer(GL_ARRAY_BUFFER, staticMatrixIDVBO);
            glBufferData(GL_ARRAY_BUFFER, staticCapacity * sizeof(unsigned int), nullptr, GL_STATIC_DRAW);
            
            std::cout << "[RenderSystem] Resized static buffers to " << staticCapacity << std::endl;
        }
        
        // Upload static data (only when dirty - handled by RenderCollector)
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, staticMaterialSSBO);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, staticMaterials.size() * sizeof(glm::vec4), staticMaterials.data());
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, staticMaterialSSBO);
        
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, staticMatrixSSBO);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, staticCount * sizeof(glm::mat4), staticMatrices.data());
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, staticMatrixSSBO);
        
        // Build static matrix IDs (1:1 mapping)
        std::vector<unsigned int> staticMatrixIDs;
        staticMatrixIDs.reserve(staticCount);
        for (size_t i = 0; i < staticCount; ++i) {
            staticMatrixIDs.push_back(static_cast<unsigned int>(i));
        }
        
        // Upload static instance IDs
        glBindBuffer(GL_ARRAY_BUFFER, staticMaterialIDVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, staticMaterialIDs.size() * sizeof(unsigned int), staticMaterialIDs.data());
        glEnableVertexAttribArray(1);
        glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(unsigned int), (void*)0);
        glVertexAttribDivisor(1, 1);
        
        glBindBuffer(GL_ARRAY_BUFFER, staticMatrixIDVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, staticMatrixIDs.size() * sizeof(unsigned int), staticMatrixIDs.data());
        glEnableVertexAttribArray(2);
        glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(unsigned int), (void*)0);
        glVertexAttribDivisor(2, 1);
        
        // Draw static instances
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, static_cast<GLsizei>(staticCount));
    }
    
    // === RENDER DYNAMIC OBJECTS ===
    if (dynamicCount > 0 && !dynamicMaterials.empty()) {
        // Resize dynamic buffers if needed (more common than static resize)
        size_t requiredDynamicCapacity = std::max(dynamicCount, dynamicMaterials.size());
        if (requiredDynamicCapacity > dynamicCapacity) {
            dynamicCapacity = requiredDynamicCapacity * 2;
            
            // Resize dynamic material SSBO
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, dynamicMaterialSSBO);
            glBufferData(GL_SHADER_STORAGE_BUFFER, dynamicCapacity * sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, dynamicMaterialSSBO);
            
            // Resize dynamic matrix SSBO
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, dynamicMatrixSSBO);
            glBufferData(GL_SHADER_STORAGE_BUFFER, dynamicCapacity * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, dynamicMatrixSSBO);
            
            // Resize dynamic material ID VBO
            glBindBuffer(GL_ARRAY_BUFFER, dynamicMaterialIDVBO);
            glBufferData(GL_ARRAY_BUFFER, dynamicCapacity * sizeof(unsigned int), nullptr, GL_DYNAMIC_DRAW);
            
            // Resize dynamic matrix ID VBO
            glBindBuffer(GL_ARRAY_BUFFER, dynamicMatrixIDVBO);
            glBufferData(GL_ARRAY_BUFFER, dynamicCapacity * sizeof(unsigned int), nullptr, GL_DYNAMIC_DRAW);
            
            std::cout << "[RenderSystem] Resized dynamic buffers to " << dynamicCapacity << std::endl;
        }
        
        // Upload dynamic data (every frame)
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, dynamicMaterialSSBO);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, dynamicMaterials.size() * sizeof(glm::vec4), dynamicMaterials.data());
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, dynamicMaterialSSBO);
        
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, dynamicMatrixSSBO);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, dynamicCount * sizeof(glm::mat4), dynamicMatrices.data());
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, dynamicMatrixSSBO);
        
        // Build dynamic matrix IDs (1:1 mapping)
        std::vector<unsigned int> dynamicMatrixIDs;
        dynamicMatrixIDs.reserve(dynamicCount);
        for (size_t i = 0; i < dynamicCount; ++i) {
            dynamicMatrixIDs.push_back(static_cast<unsigned int>(i));
        }
        
        // Upload dynamic instance IDs
        glBindBuffer(GL_ARRAY_BUFFER, dynamicMaterialIDVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, dynamicMaterialIDs.size() * sizeof(unsigned int), dynamicMaterialIDs.data());
        glEnableVertexAttribArray(1);
        glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(unsigned int), (void*)0);
        glVertexAttribDivisor(1, 1);
        
        glBindBuffer(GL_ARRAY_BUFFER, dynamicMatrixIDVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, dynamicMatrixIDs.size() * sizeof(unsigned int), dynamicMatrixIDs.data());
        glEnableVertexAttribArray(2);
        glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(unsigned int), (void*)0);
        glVertexAttribDivisor(2, 1);
        
        // Draw dynamic instances
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, static_cast<GLsizei>(dynamicCount));
    }
    
    glBindVertexArray(0);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
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
