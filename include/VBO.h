#pragma once

#include <GL/glew.h>
#include <vector>
#include <iostream>

/// Base template class for Vertex Buffer Objects using DSA (Direct State Access) API
/// Provides automatic buffer creation, resizing, and cleanup
/// Usage hint defaults to GL_STATIC_DRAW but can be customized
template<typename T>
class VBO {
private:
    GLuint bufferID = 0;
    GLenum usageHint;
    size_t capacity = 0;

public:
    /// Constructor with usage hint
    /// @param usage - OpenGL usage hint (GL_STATIC_DRAW, GL_DYNAMIC_DRAW, GL_STREAM_DRAW, etc.)
    explicit VBO(GLenum usage = GL_STATIC_DRAW)
        : usageHint(usage) {
    }

    /// Destructor - automatically cleans up buffer
    ~VBO() {
        cleanup();
    }

    /// Initialize buffer with given capacity
    /// @param initialCapacity - Number of elements to allocate
    void initialize(size_t initialCapacity) {
        if (bufferID != 0) {
            std::cerr << "[VBO] Warning: Buffer already initialized" << std::endl;
            return;
        }

        capacity = initialCapacity;
        
        // DSA: Create buffer directly without binding
        glCreateBuffers(1, &bufferID);
        glNamedBufferData(bufferID, capacity * sizeof(T), nullptr, usageHint);
        
        std::cout << "[VBO] Initialized buffer with capacity " << capacity 
                  << " (" << (capacity * sizeof(T)) << " bytes)" << std::endl;
    }

    /// Upload data to buffer (automatically resizes if needed)
    /// @param data - Vector of data to upload
    void uploadData(const std::vector<T>& data) {
        if (bufferID == 0) {
            std::cerr << "[VBO] Error: Buffer not initialized" << std::endl;
            return;
        }

        // Resize if necessary
        if (data.size() > capacity) {
            size_t newCapacity = data.size() * 2;  // Grow by 2x
            std::cout << "[VBO] Resizing buffer from " << capacity << " to " << newCapacity << std::endl;
            
            capacity = newCapacity;
            glNamedBufferData(bufferID, capacity * sizeof(T), nullptr, usageHint);
        }

        // DSA: Upload data directly without binding
        glNamedBufferSubData(bufferID, 0, data.size() * sizeof(T), data.data());
    }

    /// Bind buffer to target (traditional API - needed for some operations like vertex attrib setup)
    /// @param target - Buffer target (GL_ARRAY_BUFFER, etc.)
    void bind(GLenum target = GL_ARRAY_BUFFER) const {
        glBindBuffer(target, bufferID);
    }

    /// Unbind buffer from target
    /// @param target - Buffer target (GL_ARRAY_BUFFER, etc.)
    static void unbind(GLenum target = GL_ARRAY_BUFFER) {
        glBindBuffer(target, 0);
    }

    /// Clean up buffer resources
    void cleanup() {
        if (bufferID != 0) {
            glDeleteBuffers(1, &bufferID);
            bufferID = 0;
            capacity = 0;
        }
    }

    /// Get buffer ID
    GLuint getBufferID() const {
        return bufferID;
    }

    /// Get buffer capacity
    size_t getCapacity() const {
        return capacity;
    }

    /// Get usage hint
    GLenum getUsageHint() const {
        return usageHint;
    }

    // Move constructor
    VBO(VBO&& other) noexcept
        : bufferID(other.bufferID)
        , usageHint(other.usageHint)
        , capacity(other.capacity) {
        other.bufferID = 0;
        other.capacity = 0;
    }

    // Move assignment operator
    VBO& operator=(VBO&& other) noexcept {
        if (this != &other) {
            cleanup();
            
            bufferID = other.bufferID;
            usageHint = other.usageHint;
            capacity = other.capacity;
            
            other.bufferID = 0;
            other.capacity = 0;
        }
        return *this;
    }

    // Delete copy constructor and copy assignment
    VBO(const VBO&) = delete;
    VBO& operator=(const VBO&) = delete;
};
