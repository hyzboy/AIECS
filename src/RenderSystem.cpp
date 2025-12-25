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
        if (staticVAO != 0) {
            glDeleteVertexArrays(1, &staticVAO);
            staticVAO = 0;
        }
        if (dynamicVAO != 0) {
            glDeleteVertexArrays(1, &dynamicVAO);
            dynamicVAO = 0;
        }
        
        // Clean up using smart pointers (automatic)
        vertexVBO.reset();
        staticMaterialIDVBO.reset();
        staticMatrixIDVBO.reset();
        staticMaterialSSBO.reset();
        staticMatrixSSBO.reset();
        
        dynamicMaterialIDVBO.reset();
        dynamicMatrixIDVBO.reset();
        dynamicMaterialSSBO.reset();
        dynamicMatrixSSBO.reset();
        
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

    std::cout << "[RenderSystem] Initializing dual VAO architecture with ARB_vertex_attrib_binding..." << std::endl;

    // Create shader program
    shaderProgram = createShaderProgram();

    // Rectangle vertices from -0.5,-0.5 to +0.5,+0.5 (two triangles)
    std::vector<float> vertices = {
        // First triangle
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f,  0.5f, 0.0f,  // top left
        // Second triangle
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left
    };

    // Setup vertex buffer (position data) using VBO class - shared between static and dynamic
    vertexVBO = std::make_unique<VBO<float>>(GL_STATIC_DRAW);
    vertexVBO->initialize(vertices.size());
    vertexVBO->uploadData(vertices);
    
    // Initialize static resources (GL_STATIC_DRAW)
    staticMaterialIDVBO = std::make_unique<InstanceVBO<unsigned int>>(1, GL_STATIC_DRAW);
    staticMatrixIDVBO = std::make_unique<InstanceVBO<unsigned int>>(2, GL_STATIC_DRAW);
    staticMaterialSSBO = std::make_unique<SSBOBuffer<glm::vec4>>(0, GL_STATIC_DRAW);
    staticMatrixSSBO = std::make_unique<SSBOBuffer<glm::mat4>>(1, GL_STATIC_DRAW);
    
    staticMaterialIDVBO->initialize(100);
    staticMatrixIDVBO->initialize(100);
    staticMaterialSSBO->initialize(100);
    staticMatrixSSBO->initialize(100);
    
    // Initialize dynamic resources (GL_DYNAMIC_DRAW)
    dynamicMaterialIDVBO = std::make_unique<InstanceVBO<unsigned int>>(1, GL_DYNAMIC_DRAW);
    dynamicMatrixIDVBO = std::make_unique<InstanceVBO<unsigned int>>(2, GL_DYNAMIC_DRAW);
    dynamicMaterialSSBO = std::make_unique<SSBOBuffer<glm::vec4>>(0, GL_DYNAMIC_DRAW);
    dynamicMatrixSSBO = std::make_unique<SSBOBuffer<glm::mat4>>(1, GL_DYNAMIC_DRAW);
    
    dynamicMaterialIDVBO->initialize(100);
    dynamicMatrixIDVBO->initialize(100);
    dynamicMaterialSSBO->initialize(100);
    dynamicMatrixSSBO->initialize(100);

    // ===== CREATE STATIC VAO with ARB_vertex_attrib_binding =====
    glGenVertexArrays(1, &staticVAO);
    glBindVertexArray(staticVAO);
    
    // Define vertex format once (ARB_vertex_attrib_binding)
    // Binding 0: Position (vec3, from vertexVBO)
    glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribBinding(0, 0);  // Attribute 0 -> Binding 0
    glEnableVertexAttribArray(0);
    
    // Binding 1: Material ID (uint, from staticMaterialIDVBO)
    glVertexAttribIFormat(1, 1, GL_UNSIGNED_INT, 0);
    glVertexAttribBinding(1, 1);  // Attribute 1 -> Binding 1
    glEnableVertexAttribArray(1);
    
    // Binding 2: Matrix ID (uint, from staticMatrixIDVBO)
    glVertexAttribIFormat(2, 1, GL_UNSIGNED_INT, 0);
    glVertexAttribBinding(2, 2);  // Attribute 2 -> Binding 2
    glEnableVertexAttribArray(2);
    
    // Bind buffers to binding points (ARB_vertex_attrib_binding)
    glBindVertexBuffer(0, vertexVBO->getBufferID(), 0, 3 * sizeof(float));  // Binding 0: position, divisor 0 (per-vertex)
    glBindVertexBuffer(1, staticMaterialIDVBO->getBufferID(), 0, sizeof(unsigned int));  // Binding 1: materialID, divisor 1 (per-instance)
    glBindVertexBuffer(2, staticMatrixIDVBO->getBufferID(), 0, sizeof(unsigned int));  // Binding 2: matrixID, divisor 1 (per-instance)
    
    // Set divisors (0 = per-vertex, 1 = per-instance)
    glVertexBindingDivisor(0, 0);  // Position: per-vertex
    glVertexBindingDivisor(1, 1);  // Material ID: per-instance
    glVertexBindingDivisor(2, 1);  // Matrix ID: per-instance
    
    // ===== CREATE DYNAMIC VAO with ARB_vertex_attrib_binding =====
    glGenVertexArrays(1, &dynamicVAO);
    glBindVertexArray(dynamicVAO);
    
    // Define vertex format once (same as static VAO)
    glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribBinding(0, 0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribIFormat(1, 1, GL_UNSIGNED_INT, 0);
    glVertexAttribBinding(1, 1);
    glEnableVertexAttribArray(1);
    
    glVertexAttribIFormat(2, 1, GL_UNSIGNED_INT, 0);
    glVertexAttribBinding(2, 2);
    glEnableVertexAttribArray(2);
    
    // Bind buffers to binding points (dynamic buffers)
    glBindVertexBuffer(0, vertexVBO->getBufferID(), 0, 3 * sizeof(float));  // Binding 0: position (shared)
    glBindVertexBuffer(1, dynamicMaterialIDVBO->getBufferID(), 0, sizeof(unsigned int));  // Binding 1: materialID
    glBindVertexBuffer(2, dynamicMatrixIDVBO->getBufferID(), 0, sizeof(unsigned int));  // Binding 2: matrixID
    
    // Set divisors
    glVertexBindingDivisor(0, 0);  // Position: per-vertex
    glVertexBindingDivisor(1, 1);  // Material ID: per-instance
    glVertexBindingDivisor(2, 1);  // Matrix ID: per-instance

    glBindVertexArray(0);

    // Set up orthographic projection for 2D rendering
    projectionMatrix = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);
    
    glUseProgram(shaderProgram);
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    glInitialized = true;
    std::cout << "[RenderSystem] Dual VAO architecture with ARB_vertex_attrib_binding initialization complete." << std::endl;
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
    
    // ===== RENDER STATIC OBJECTS using staticVAO =====
    if (staticCount > 0 && !staticMaterials.empty()) {
        // Upload static data ONLY ONCE (GL_STATIC_DRAW optimization)
        if (!staticDataUploaded) {
            // Upload static data using helper classes (automatic resizing)
            staticMaterialSSBO->uploadData(staticMaterials);
            staticMatrixSSBO->uploadData(staticMatrices);
            staticMaterialIDVBO->uploadData(staticMaterialIDs);
            
            // Build static matrix IDs (1:1 mapping)
            std::vector<unsigned int> staticMatrixIDs_data;
            staticMatrixIDs_data.reserve(staticCount);
            for (size_t i = 0; i < staticCount; ++i) {
                staticMatrixIDs_data.push_back(static_cast<unsigned int>(i));
            }
            staticMatrixIDVBO->uploadData(staticMatrixIDs_data);
            
            staticDataUploaded = true;
            std::cout << "[RenderSystem] Static data uploaded once (GL_STATIC_DRAW): " 
                      << staticCount << " instances, " 
                      << staticMaterials.size() << " materials" << std::endl;
        }
        
        // Bind static VAO (ARB_vertex_attrib_binding: all buffers already bound!)
        glBindVertexArray(staticVAO);
        
        // Bind SSBOs (binding is needed every frame even if data doesn't change)
        staticMaterialSSBO->bind();
        staticMatrixSSBO->bind();
        
        // Draw static instances (no attribute setup needed - VAO has everything!)
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, static_cast<GLsizei>(staticCount));
    }
    
    // ===== RENDER DYNAMIC OBJECTS using dynamicVAO =====
    if (dynamicCount > 0 && !dynamicMaterials.empty()) {
        // Upload dynamic data using helper classes (automatic resizing, every frame)
        dynamicMaterialSSBO->uploadData(dynamicMaterials);
        dynamicMatrixSSBO->uploadData(dynamicMatrices);
        dynamicMaterialIDVBO->uploadData(dynamicMaterialIDs);
        
        // Build dynamic matrix IDs (1:1 mapping)
        std::vector<unsigned int> dynamicMatrixIDs_data;
        dynamicMatrixIDs_data.reserve(dynamicCount);
        for (size_t i = 0; i < dynamicCount; ++i) {
            dynamicMatrixIDs_data.push_back(static_cast<unsigned int>(i));
        }
        dynamicMatrixIDVBO->uploadData(dynamicMatrixIDs_data);
        
        // Bind dynamic VAO (ARB_vertex_attrib_binding: all buffers already bound!)
        glBindVertexArray(dynamicVAO);
        
        // Bind SSBOs
        dynamicMaterialSSBO->bind();
        dynamicMatrixSSBO->bind();
        
        // Draw dynamic instances (no attribute setup needed - VAO has everything!)
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
