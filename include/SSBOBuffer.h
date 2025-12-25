#pragma once

#include <GL/glew.h>
#include <vector>
#include <cstring>

/// Encapsulated SSBO (Shader Storage Buffer Object) class using DSA (Direct State Access) API
/// Simplifies buffer management with automatic resizing and usage hint support
/// Uses modern OpenGL 4.5+ DSA functions for stateless, efficient buffer operations
/// Supports persistent mapped buffers (OpenGL 4.4+) for zero-copy updates
template<typename T>
class SSBOBuffer {
public:
    /// Constructor
    /// @param binding - SSBO binding point (0-N)
    /// @param usageHint - GL_STATIC_DRAW or GL_DYNAMIC_DRAW
    /// @param persistent - Enable persistent mapped buffers for zero-copy updates (requires GL 4.4+)
    SSBOBuffer(GLuint binding, GLenum usageHint = GL_DYNAMIC_DRAW, bool persistent = false)
        : bindingPoint(binding), usage(usageHint), usePersistentMapping(persistent) {
    }

    ~SSBOBuffer() {
        cleanup();
    }

    // Prevent copying
    SSBOBuffer(const SSBOBuffer&) = delete;
    SSBOBuffer& operator=(const SSBOBuffer&) = delete;

    // Allow moving
    SSBOBuffer(SSBOBuffer&& other) noexcept
        : bufferID(other.bufferID), bindingPoint(other.bindingPoint),
          usage(other.usage), capacity(other.capacity),
          usePersistentMapping(other.usePersistentMapping), mappedPtr(other.mappedPtr) {
        other.bufferID = 0;
        other.capacity = 0;
        other.mappedPtr = nullptr;
    }

    SSBOBuffer& operator=(SSBOBuffer&& other) noexcept {
        if (this != &other) {
            cleanup();
            bufferID = other.bufferID;
            bindingPoint = other.bindingPoint;
            usage = other.usage;
            capacity = other.capacity;
            usePersistentMapping = other.usePersistentMapping;
            mappedPtr = other.mappedPtr;
            other.bufferID = 0;
            other.capacity = 0;
            other.mappedPtr = nullptr;
        }
        return *this;
    }

    /// Initialize the SSBO with an initial capacity (DSA)
    void initialize(size_t initialCapacity = 100) {
        if (bufferID != 0) {
            return;  // Already initialized
        }

        capacity = initialCapacity;
        
        // DSA: Create buffer directly without binding
        glCreateBuffers(1, &bufferID);
        
        if (usePersistentMapping) {
            // Use persistent mapped buffer (GL 4.4+)
            GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
            glNamedBufferStorage(bufferID, capacity * sizeof(T), nullptr, flags);
            
            // Map the buffer persistently
            mappedPtr = static_cast<T*>(glMapNamedBufferRange(bufferID, 0, capacity * sizeof(T), flags));
            
            if (!mappedPtr) {
                usePersistentMapping = false;  // Fallback to traditional
            }
        } else {
            // Traditional buffer
            // DSA: Allocate buffer storage directly
            glNamedBufferData(bufferID, capacity * sizeof(T), nullptr, usage);
        }
    }

    /// Upload data to the SSBO (DSA)
    /// Automatically resizes if needed
    void uploadData(const std::vector<T>& data) {
        if (bufferID == 0) {
            initialize();
        }

        // Handle persistent mapped buffers
        if (usePersistentMapping && mappedPtr) {
            if (data.size() > capacity) {
                // Cannot resize persistent mapped buffers
                return;
            }
            
            if (!data.empty()) {
                // Zero-copy: Direct memory write to mapped buffer
                std::memcpy(mappedPtr, data.data(), data.size() * sizeof(T));
                // No explicit sync needed with GL_MAP_COHERENT_BIT
            }
        } else {
            // Traditional path: Resize if needed
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
    }

    /// Bind the SSBO to its binding point
    /// Note: glBindBufferBase still needed for shader binding (not part of DSA)
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
    
    /// Check if using persistent mapping
    bool isPersistentlyMapped() const {
        return usePersistentMapping && mappedPtr != nullptr;
    }
    
    /// Get mapped pointer (for advanced use cases)
    /// Returns nullptr if not persistently mapped
    T* getMappedPointer() {
        return mappedPtr;
    }

    /// Cleanup resources
    void cleanup() {
        if (bufferID != 0) {
            // Unmap if persistently mapped
            if (mappedPtr) {
                glUnmapNamedBuffer(bufferID);
                mappedPtr = nullptr;
            }
            
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
    bool usePersistentMapping = false;
    T* mappedPtr = nullptr;
};
