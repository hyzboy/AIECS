#include "TransformComputeSystem.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// Compute shader for flat hierarchy transform calculation
static const char* computeShaderSource = R"(
#version 430 core

layout(local_size_x = 256) in;

// Input buffers: TRS data
layout(std430, binding = 0) buffer PositionBuffer {
    vec4 positions[];  // vec3 padded to vec4
};

layout(std430, binding = 1) buffer RotationBuffer {
    vec4 rotations[];  // quaternion (x, y, z, w)
};

layout(std430, binding = 2) buffer ScaleBuffer {
    vec4 scales[];  // vec3 padded to vec4
};

layout(std430, binding = 3) buffer ParentIndexBuffer {
    uint parentIndices[];  // parent index (0xFFFFFFFF = root)
};

// Output buffer: World matrices
layout(std430, binding = 4) buffer WorldMatrixBuffer {
    mat4 worldMatrices[];
};

// Convert quaternion to rotation matrix
mat4 quatToMat4(vec4 q) {
    float xx = q.x * q.x;
    float yy = q.y * q.y;
    float zz = q.z * q.z;
    float xy = q.x * q.y;
    float xz = q.x * q.z;
    float yz = q.y * q.z;
    float wx = q.w * q.x;
    float wy = q.w * q.y;
    float wz = q.w * q.z;
    
    mat4 m;
    m[0][0] = 1.0 - 2.0 * (yy + zz);
    m[0][1] = 2.0 * (xy + wz);
    m[0][2] = 2.0 * (xz - wy);
    m[0][3] = 0.0;
    
    m[1][0] = 2.0 * (xy - wz);
    m[1][1] = 1.0 - 2.0 * (xx + zz);
    m[1][2] = 2.0 * (yz + wx);
    m[1][3] = 0.0;
    
    m[2][0] = 2.0 * (xz + wy);
    m[2][1] = 2.0 * (yz - wx);
    m[2][2] = 1.0 - 2.0 * (xx + yy);
    m[2][3] = 0.0;
    
    m[3][0] = 0.0;
    m[3][1] = 0.0;
    m[3][2] = 0.0;
    m[3][3] = 1.0;
    
    return m;
}

// Build TRS matrix
mat4 buildTRSMatrix(vec3 pos, vec4 rot, vec3 scale) {
    // Scale matrix
    mat4 S = mat4(1.0);
    S[0][0] = scale.x;
    S[1][1] = scale.y;
    S[2][2] = scale.z;
    
    // Rotation matrix from quaternion
    mat4 R = quatToMat4(rot);
    
    // Translation matrix
    mat4 T = mat4(1.0);
    T[3][0] = pos.x;
    T[3][1] = pos.y;
    T[3][2] = pos.z;
    
    // TRS = T * R * S
    return T * R * S;
}

void main() {
    uint idx = gl_GlobalInvocationID.x;
    
    // Bounds check
    if (idx >= positions.length()) {
        return;
    }
    
    // Build local TRS matrix
    vec3 pos = positions[idx].xyz;
    vec4 rot = rotations[idx];
    vec3 scale = scales[idx].xyz;
    mat4 localMatrix = buildTRSMatrix(pos, rot, scale);
    
    // Check for parent
    uint parentIdx = parentIndices[idx];
    
    if (parentIdx == 0xFFFFFFFF) {
        // Root entity - world matrix = local matrix
        worldMatrices[idx] = localMatrix;
    } else {
        // Has parent - world matrix = parent world * local matrix
        // Note: This is a flat hierarchy, so parent must be computed first
        // For true hierarchies, need multiple passes or reordering
        worldMatrices[idx] = worldMatrices[parentIdx] * localMatrix;
    }
}
)";

TransformComputeSystem::TransformComputeSystem(const std::string& name)
    : EntitySystem(name) {
}

TransformComputeSystem::~TransformComputeSystem() {
    shutdown();
}

void TransformComputeSystem::initialize() {
    std::cout << "[TransformComputeSystem] Initializing GPU compute transform system..." << std::endl;
}

void TransformComputeSystem::update(float deltaTime) {
    // Compute shader is dispatched manually via computeWorldMatrices()
    // No per-frame update needed unless automatic mode is desired
}

void TransformComputeSystem::shutdown() {
    computeProgram.reset();
    
    // SSBOs auto-cleaned by smart pointers
    positionSSBO.reset();
    rotationSSBO.reset();
    scaleSSBO.reset();
    parentIndexSSBO.reset();
    worldMatrixSSBO.reset();
    
    glInitialized = false;
}

void TransformComputeSystem::initializeGL() {
    if (glInitialized) return;
    
    std::cout << "[TransformComputeSystem] Creating compute shader program..." << std::endl;
    computeProgram = std::make_unique<ShaderProgram>();
    if (!computeProgram->createFromCompute(computeShaderSource)) {
        std::cerr << "[TransformComputeSystem] Failed to create compute shader program" << std::endl;
        return;
    }
    
    // Create SSBOs with persistent mapping for zero-copy updates
    positionSSBO = std::make_unique<SSBOBuffer<glm::vec4>>(0, GL_DYNAMIC_DRAW, true);
    rotationSSBO = std::make_unique<SSBOBuffer<glm::vec4>>(1, GL_DYNAMIC_DRAW, true);
    scaleSSBO = std::make_unique<SSBOBuffer<glm::vec4>>(2, GL_DYNAMIC_DRAW, true);
    parentIndexSSBO = std::make_unique<SSBOBuffer<uint32_t>>(3, GL_DYNAMIC_DRAW, true);
    worldMatrixSSBO = std::make_unique<SSBOBuffer<glm::mat4>>(4, GL_DYNAMIC_DRAW, false);
    
    glInitialized = true;
    std::cout << "[TransformComputeSystem] GPU compute system initialized" << std::endl;
}

void TransformComputeSystem::uploadTransformData(const std::vector<glm::vec3>& positions,
                                                const std::vector<glm::quat>& rotations,
                                                const std::vector<glm::vec3>& scales,
                                                const std::vector<uint32_t>& parentIndices) {
    if (!glInitialized) {
        initializeGL();
    }
    
    transformCount = positions.size();
    
    // Convert vec3 to vec4 (padding for SSBO alignment)
    std::vector<glm::vec4> posVec4(positions.size());
    std::vector<glm::vec4> scaleVec4(scales.size());
    std::vector<glm::vec4> rotVec4(rotations.size());
    
    for (size_t i = 0; i < positions.size(); ++i) {
        posVec4[i] = glm::vec4(positions[i], 1.0f);
        scaleVec4[i] = glm::vec4(scales[i], 1.0f);
        rotVec4[i] = glm::vec4(rotations[i].x, rotations[i].y, rotations[i].z, rotations[i].w);
    }
    
    // Upload to GPU (zero-copy with persistent mapping)
    positionSSBO->uploadData(posVec4);
    rotationSSBO->uploadData(rotVec4);
    scaleSSBO->uploadData(scaleVec4);
    parentIndexSSBO->uploadData(parentIndices);
    
    // Allocate output buffer
    std::vector<glm::mat4> emptyMatrices(transformCount, glm::mat4(1.0f));
    worldMatrixSSBO->uploadData(emptyMatrices);
}

void TransformComputeSystem::computeWorldMatrices() {
    if (!glInitialized || !enabled || transformCount == 0) {
        return;
    }
    
    computeProgram->use();
    
    // Bind SSBOs
    positionSSBO->bind();
    rotationSSBO->bind();
    scaleSSBO->bind();
    parentIndexSSBO->bind();
    worldMatrixSSBO->bind();
    
    // Dispatch compute shader
    // Work group size: 256 threads per group
    GLuint numGroups = (transformCount + 255) / 256;
    glDispatchCompute(numGroups, 1, 1);
    
    // Memory barrier to ensure writes are visible
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

// Shader compilation/linking moved to shared ShaderProgram implementation
