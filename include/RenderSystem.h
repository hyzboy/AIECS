#pragma once

#include "Module.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

/// Dedicated rendering module for drawing 2D rectangles
/// Handles all OpenGL rendering operations
class RenderSystem : public Module {
public:
    RenderSystem(const std::string& name = "RenderSystem");
    ~RenderSystem() override;

    void initialize() override;
    void update(float deltaTime) override;
    void shutdown() override;

    /// Initialize OpenGL resources (shaders, VAO/VBO)
    void initializeGL();

    /// Render a batch of rectangles using instanced rendering
    /// @param modelMatrices - World transform matrices for each rectangle
    /// @param colors - Colors for each rectangle
    void renderBatch(const std::vector<glm::mat4>& modelMatrices, 
                     const std::vector<glm::vec4>& colors);

    /// Get shader program ID
    unsigned int getShaderProgram() const { return shaderProgram; }

private:
    unsigned int compileShader(GLenum type, const char* source);
    unsigned int createShaderProgram();
    void setupInstanceBuffers();

    // OpenGL resources
    unsigned int shaderProgram = 0;
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int instanceMatrixVBO = 0;  // VBO for instance matrices
    unsigned int instanceColorVBO = 0;   // VBO for instance colors
    bool glInitialized = false;

    // Projection matrix for 2D rendering
    glm::mat4 projectionMatrix;
    
    // Instance buffer capacity
    size_t instanceBufferCapacity = 100;
};
