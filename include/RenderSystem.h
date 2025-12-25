#pragma once

#include "Module.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

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
    unsigned int staticMaterialIDVBO = 0;    // Static material IDs
    unsigned int staticMatrixIDVBO = 0;      // Static matrix IDs
    unsigned int staticMaterialSSBO = 0;     // Static materials SSBO
    unsigned int staticMatrixSSBO = 0;       // Static matrices SSBO
    
    // Dynamic data resources (GL_DYNAMIC_DRAW - updated every frame)
    unsigned int dynamicMaterialIDVBO = 0;   // Dynamic material IDs
    unsigned int dynamicMatrixIDVBO = 0;     // Dynamic matrix IDs
    unsigned int dynamicMaterialSSBO = 0;    // Dynamic materials SSBO
    unsigned int dynamicMatrixSSBO = 0;      // Dynamic matrices SSBO
    
    bool glInitialized = false;

    // Projection matrix for 2D rendering
    glm::mat4 projectionMatrix;
    
    // Buffer capacities
    size_t staticCapacity = 100;
    size_t dynamicCapacity = 100;
};
