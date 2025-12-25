#pragma once

#include <GL/glew.h>
#include <vector>

/// Encapsulated Instance VBO (Vertex Buffer Object) class using DSA (Direct State Access) API
/// Simplifies instance buffer management with automatic resizing and usage hint support
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

    // Prevent copying
    InstanceVBO(const InstanceVBO&) = delete;
    InstanceVBO& operator=(const InstanceVBO&) = delete;

    // Allow moving
    InstanceVBO(InstanceVBO&& other) noexcept
        : bufferID(other.bufferID), attributeLocation(other.attributeLocation),
          usage(other.usage), capacity(other.capacity) {
        other.bufferID = 0;
        other.capacity = 0;
    }

    InstanceVBO& operator=(InstanceVBO&& other) noexcept {
        if (this != &other) {
            cleanup();
            bufferID = other.bufferID;
            attributeLocation = other.attributeLocation;
            usage = other.usage;
            capacity = other.capacity;
            other.bufferID = 0;
            other.capacity = 0;
        }
        return *this;
    }

    /// Initialize the VBO with an initial capacity (DSA)
    void initialize(size_t initialCapacity = 100) {
        if (bufferID != 0) {
            return;  // Already initialized
        }

        capacity = initialCapacity;
        
        // DSA: Create buffer directly without binding
        glCreateBuffers(1, &bufferID);
        
        // DSA: Allocate buffer storage directly
        glNamedBufferData(bufferID, capacity * sizeof(T), nullptr, usage);
    }

    /// Upload data to the VBO (DSA)
    /// Automatically resizes if needed
    void uploadData(const std::vector<T>& data) {
        if (bufferID == 0) {
            initialize();
        }

        // Resize if needed
        if (data.size() > capacity) {
            capacity = data.size() * 2;  // Double capacity
            
            // DSA: Reallocate buffer directly
            glNamedBufferData(bufferID, capacity * sizeof(T), nullptr, usage);
        }

        // Upload data
        if (!data.empty()) {
            // DSA: Upload data directly without binding
            glNamedBufferSubData(bufferID, 0, data.size() * sizeof(T), data.data());
        }
    }

    /// Setup vertex attribute pointer for instanced rendering
    /// Call this after binding VAO (still requires traditional API as it's VAO-dependent)
    void setupAttribute(GLint componentsPerAttribute = 1, bool isInteger = true) {
        if (bufferID == 0) {
            return;
        }

        // Note: VAO-related calls still use traditional API
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
    
    /// Get buffer ID (alias for consistency with VBO)
    GLuint getBufferID() const { return bufferID; }

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
