#pragma once

#include <GL/glew.h>
#include <vector>
#include <cstring>

/// Encapsulated SSBO (Shader Storage Buffer Object) class
/// Simplifies buffer management with automatic resizing and usage hint support
template<typename T>
class SSBOBuffer {
public:
    /// Constructor
    /// @param binding - SSBO binding point (0-N)
    /// @param usageHint - GL_STATIC_DRAW or GL_DYNAMIC_DRAW
    SSBOBuffer(GLuint binding, GLenum usageHint = GL_DYNAMIC_DRAW)
        : bindingPoint(binding), usage(usageHint) {
    }

    ~SSBOBuffer() {
        cleanup();
    }

    /// Initialize the SSBO with an initial capacity
    void initialize(size_t initialCapacity = 100) {
        if (bufferID != 0) {
            return;  // Already initialized
        }

        capacity = initialCapacity;
        glGenBuffers(1, &bufferID);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID);
        glBufferData(GL_SHADER_STORAGE_BUFFER, capacity * sizeof(T), nullptr, usage);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    /// Upload data to the SSBO
    /// Automatically resizes if needed
    void uploadData(const std::vector<T>& data) {
        if (bufferID == 0) {
            initialize();
        }

        // Resize if needed
        if (data.size() > capacity) {
            capacity = data.size() * 2;  // Double capacity
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID);
            glBufferData(GL_SHADER_STORAGE_BUFFER, capacity * sizeof(T), nullptr, usage);
        }

        // Upload data
        if (!data.empty()) {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID);
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, data.size() * sizeof(T), data.data());
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }
    }

    /// Bind the SSBO to its binding point
    void bind() const {
        if (bufferID != 0) {
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, bufferID);
        }
    }

    /// Unbind the SSBO
    void unbind() const {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, 0);
    }

    /// Get buffer ID
    GLuint getID() const { return bufferID; }

    /// Get binding point
    GLuint getBindingPoint() const { return bindingPoint; }

    /// Get capacity
    size_t getCapacity() const { return capacity; }

    /// Cleanup resources
    void cleanup() {
        if (bufferID != 0) {
            glDeleteBuffers(1, &bufferID);
            bufferID = 0;
        }
        capacity = 0;
    }

private:
    GLuint bufferID = 0;
    GLuint bindingPoint = 0;
    GLenum usage = GL_DYNAMIC_DRAW;
    size_t capacity = 0;
};

/// Specialized VBO buffer for instance data
/// Similar to SSBO but uses vertex array buffer target
template<typename T>
class InstanceVBO {
public:
    /// Constructor
    /// @param location - Vertex attribute location
    /// @param usageHint - GL_STATIC_DRAW or GL_DYNAMIC_DRAW
    InstanceVBO(GLuint location, GLenum usageHint = GL_DYNAMIC_DRAW)
        : attributeLocation(location), usage(usageHint) {
    }

    ~InstanceVBO() {
        cleanup();
    }

    /// Initialize the VBO with an initial capacity
    void initialize(size_t initialCapacity = 100) {
        if (bufferID != 0) {
            return;  // Already initialized
        }

        capacity = initialCapacity;
        glGenBuffers(1, &bufferID);
        glBindBuffer(GL_ARRAY_BUFFER, bufferID);
        glBufferData(GL_ARRAY_BUFFER, capacity * sizeof(T), nullptr, usage);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    /// Upload data to the VBO
    /// Automatically resizes if needed
    void uploadData(const std::vector<T>& data) {
        if (bufferID == 0) {
            initialize();
        }

        // Resize if needed
        if (data.size() > capacity) {
            capacity = data.size() * 2;  // Double capacity
            glBindBuffer(GL_ARRAY_BUFFER, bufferID);
            glBufferData(GL_ARRAY_BUFFER, capacity * sizeof(T), nullptr, usage);
        }

        // Upload data
        if (!data.empty()) {
            glBindBuffer(GL_ARRAY_BUFFER, bufferID);
            glBufferSubData(GL_ARRAY_BUFFER, 0, data.size() * sizeof(T), data.data());
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }

    /// Setup vertex attribute pointer for instanced rendering
    /// Call this after binding VAO
    void setupAttribute(GLint componentsPerAttribute = 1, bool isInteger = true) {
        if (bufferID == 0) {
            return;
        }

        glBindBuffer(GL_ARRAY_BUFFER, bufferID);
        
        if (isInteger) {
            // For integer types (e.g., uint)
            glVertexAttribIPointer(attributeLocation, componentsPerAttribute, GL_UNSIGNED_INT, 0, nullptr);
        } else {
            // For float types
            glVertexAttribPointer(attributeLocation, componentsPerAttribute, GL_FLOAT, GL_FALSE, 0, nullptr);
        }
        
        glEnableVertexAttribArray(attributeLocation);
        glVertexAttribDivisor(attributeLocation, 1);  // Per-instance data
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    /// Get buffer ID
    GLuint getID() const { return bufferID; }

    /// Get attribute location
    GLuint getLocation() const { return attributeLocation; }

    /// Get capacity
    size_t getCapacity() const { return capacity; }

    /// Cleanup resources
    void cleanup() {
        if (bufferID != 0) {
            glDeleteBuffers(1, &bufferID);
            bufferID = 0;
        }
        capacity = 0;
    }

private:
    GLuint bufferID = 0;
    GLuint attributeLocation = 0;
    GLenum usage = GL_DYNAMIC_DRAW;
    size_t capacity = 0;
};
