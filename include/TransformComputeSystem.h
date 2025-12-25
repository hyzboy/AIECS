#pragma once

#include "Module.h"
#include "SSBOBuffer.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <memory>

/// GPU-driven flat hierarchy transform system using Compute Shaders
/// Computes world matrices on GPU from TRS + parent index data
/// Perfect for CPU粗粒度 + GPU细粒度 architecture
class TransformComputeSystem : public Module {
public:
    TransformComputeSystem(const std::string& name = "TransformComputeSystem");
    ~TransformComputeSystem() override;

    void initialize() override;
    void update(float deltaTime) override;
    void shutdown() override;

    /// Initialize compute shader and GPU buffers
    void initializeGL();

    /// Upload flat transform data (TRS + parent indices) to GPU
    /// @param positions - Entity positions
    /// @param rotations - Entity rotations (quaternions)
    /// @param scales - Entity scales
    /// @param parentIndices - Parent entity indices (UINT32_MAX = root)
    void uploadTransformData(const std::vector<glm::vec3>& positions,
                            const std::vector<glm::quat>& rotations,
                            const std::vector<glm::vec3>& scales,
                            const std::vector<uint32_t>& parentIndices);

    /// Dispatch compute shader to calculate world matrices on GPU
    /// Result stored in worldMatrixSSBO, ready for rendering
    void computeWorldMatrices();

    /// Get computed world matrix SSBO (for binding to rendering)
    GLuint getWorldMatrixSSBO() const { 
        return worldMatrixSSBO ? worldMatrixSSBO->getBufferID() : 0; 
    }

    /// Get number of transforms
    size_t getTransformCount() const { return transformCount; }

    /// Enable/disable GPU compute (for performance comparison)
    void setEnabled(bool enabled) { this->enabled = enabled; }
    bool isEnabled() const { return enabled; }

private:
    unsigned int compileComputeShader(const char* source);
    unsigned int createComputeProgram();

    // Compute shader program
    unsigned int computeProgram = 0;

    // Input SSBOs (CPU → GPU) - TRS data
    std::unique_ptr<SSBOBuffer<glm::vec4>> positionSSBO;      // Binding 0: vec3 positions (padded to vec4)
    std::unique_ptr<SSBOBuffer<glm::vec4>> rotationSSBO;      // Binding 1: vec4 rotations (quaternions)
    std::unique_ptr<SSBOBuffer<glm::vec4>> scaleSSBO;         // Binding 2: vec3 scales (padded to vec4)
    std::unique_ptr<SSBOBuffer<uint32_t>> parentIndexSSBO;    // Binding 3: uint parent indices

    // Output SSBO (GPU → Rendering) - Computed world matrices
    std::unique_ptr<SSBOBuffer<glm::mat4>> worldMatrixSSBO;   // Binding 4: mat4 world matrices

    size_t transformCount = 0;
    bool glInitialized = false;
    bool enabled = true;  // Enable GPU compute by default
};
