#pragma once

#include "Module.h"
#include "SSBOBuffer.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>

/// Dedicated rendering module for drawing 2D rectangles using SSBO-based rendering
/// Handles all OpenGL rendering operations
class RenderSystem : public Module {
public:
    RenderSystem(const std::string& name = "RenderSystem");
    ~RenderSystem() override;

    void initialize() override;
    void update(float deltaTime) override;
    void shutdown() override;

    /// Initialize OpenGL resources (shaders, VAO/VBO, SSBOs)
    void initializeGL();

    /// Render a batch of rectangles using dual SSBO/VBO architecture for static and dynamic data
    /// @param staticMatrices - Static/Stationary transform matrices
    /// @param staticMaterials - Static materials (colors)
    /// @param staticMaterialIDs - Material IDs for static instances
    /// @param dynamicMatrices - Movable transform matrices
    /// @param dynamicMaterials - Dynamic materials (colors)
    /// @param dynamicMaterialIDs - Material IDs for dynamic instances
    void renderBatch(const std::vector<glm::mat4>& staticMatrices,
                     const std::vector<glm::vec4>& staticMaterials,
                     const std::vector<unsigned int>& staticMaterialIDs,
                     const std::vector<glm::mat4>& dynamicMatrices,
                     const std::vector<glm::vec4>& dynamicMaterials,
                     const std::vector<unsigned int>& dynamicMaterialIDs);

    /// Get shader program ID
    unsigned int getShaderProgram() const { return shaderProgram; }

private:
    unsigned int compileShader(GLenum type, const char* source);
    unsigned int createShaderProgram();

    // OpenGL resources
    unsigned int shaderProgram = 0;
    unsigned int VAO = 0;
    unsigned int VBO = 0;  // Vertex positions (shared)
    
    // Static data resources (GL_STATIC_DRAW - rarely updated)
    std::unique_ptr<InstanceVBO<unsigned int>> staticMaterialIDVBO;    // Static material IDs
    std::unique_ptr<InstanceVBO<unsigned int>> staticMatrixIDVBO;      // Static matrix IDs
    std::unique_ptr<SSBOBuffer<glm::vec4>> staticMaterialSSBO;         // Static materials SSBO
    std::unique_ptr<SSBOBuffer<glm::mat4>> staticMatrixSSBO;           // Static matrices SSBO
    
    // Dynamic data resources (GL_DYNAMIC_DRAW - updated every frame)
    std::unique_ptr<InstanceVBO<unsigned int>> dynamicMaterialIDVBO;   // Dynamic material IDs
    std::unique_ptr<InstanceVBO<unsigned int>> dynamicMatrixIDVBO;     // Dynamic matrix IDs
    std::unique_ptr<SSBOBuffer<glm::vec4>> dynamicMaterialSSBO;        // Dynamic materials SSBO
    std::unique_ptr<SSBOBuffer<glm::mat4>> dynamicMatrixSSBO;          // Dynamic matrices SSBO
    
    bool glInitialized = false;

    // Projection matrix for 2D rendering
    glm::mat4 projectionMatrix;
};
