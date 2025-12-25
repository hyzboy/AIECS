#pragma once

#include <GL/glew.h>
#include <vector>
#include <memory>

/// Vertex Array Object wrapper with ARB_vertex_attrib_binding support
/// Encapsulates VAO management with modern OpenGL separate vertex format architecture
class VAO {
public:
    /// Constructor - creates VAO
    VAO();
    
    /// Destructor - cleans up VAO
    ~VAO();
    
    /// Move constructor
    VAO(VAO&& other) noexcept;
    
    /// Move assignment
    VAO& operator=(VAO&& other) noexcept;
    
    /// Prevent copying
    VAO(const VAO&) = delete;
    VAO& operator=(const VAO&) = delete;
    
    /// Initialize VAO (creates if not already created)
    void initialize();
    
    /// Bind this VAO for rendering
    void bind() const;
    
    /// Unbind current VAO
    static void unbind();
    
    /// Setup a float attribute with ARB_vertex_attrib_binding
    /// @param attributeLocation - Shader attribute location
    /// @param bindingPoint - Buffer binding point
    /// @param componentCount - Number of components (1-4)
    /// @param normalized - Whether to normalize data
    void setupFloatAttribute(GLuint attributeLocation, GLuint bindingPoint, 
                            GLint componentCount, GLboolean normalized = GL_FALSE);
    
    /// Setup an integer attribute with ARB_vertex_attrib_binding
    /// @param attributeLocation - Shader attribute location
    /// @param bindingPoint - Buffer binding point
    /// @param componentCount - Number of components (1-4)
    void setupIntegerAttribute(GLuint attributeLocation, GLuint bindingPoint, 
                              GLint componentCount);
    
    /// Bind a buffer to a binding point with ARB_vertex_attrib_binding
    /// @param bindingPoint - Buffer binding point
    /// @param bufferID - OpenGL buffer ID
    /// @param offset - Offset in bytes into the buffer
    /// @param stride - Stride in bytes between elements
    void bindVertexBuffer(GLuint bindingPoint, GLuint bufferID, 
                         GLintptr offset, GLsizei stride);
    
    /// Set binding divisor (0 = per-vertex, 1 = per-instance)
    /// @param bindingPoint - Buffer binding point
    /// @param divisor - Divisor value
    void setBindingDivisor(GLuint bindingPoint, GLuint divisor);
    
    /// Get VAO ID
    GLuint getVAOID() const { return vaoID; }
    
    /// Cleanup VAO
    void cleanup();
    
private:
    GLuint vaoID = 0;
};
